import { UIMessage } from '@messages/sender'

// --- 全局状态管理 ---
let discoveredImageNodes: SceneNode[] = [];
let processedComponentFamilyIds = new Set<string>();
// [更新] 定义特殊关键字列表，用于以下场景（不区分大小写）：
// 1. [特殊FRAME处理]: 如果一个 FRAME 节点的 name 包含列表中的关键字，
//    - a) 它在 XML 中会转换为以匹配到的第一个关键字命名的新标签（例如 <LIST>）。
//    - b) 它的所有子节点都不会出现在 XML 中。
//    - c) 程序会深入其子节点搜集图片资源，但遵循下面的第2条规则。
// 2. [资源收集时跳过]: 在执行特殊的资源搜集时，如果遇到任何一个后代节点的 name 完全匹配列表中的关键字，
//    那么这个后代节点及其所有子树将被完全跳过，不再向下查找图片。
const SPECIAL_KEYWORD_NAMES = new Set(['cellular', 'list', 'video']);


// 显示插件UI
mg.showUI(__html__)

/**
 * 主消息处理器，负责监听并响应来自UI界面的事件。
 */
mg.ui.onmessage = async (msg) => {
  console.log('核心逻辑: 收到来自 UI 的消息', msg);

  const currentPage = mg.document.currentPage;
  const selectedNodes = currentPage.selection;

  if (selectedNodes.length === 0) {
    const notification = "请先在画板上选中至少一个图层。";
    mg.notify(notification);
    if (msg.pluginMessage.type === 'generate-xml') {
      mg.ui.postMessage({ pluginMessage:{ type: 'xml-generated', data: '<!-- ' + notification + ' -->' } });
    }
    return;
  }
  console.log(`msg.pluginMessage.type ${msg.pluginMessage.type}`);
  // --- 事件路由：处理“生成 XML”请求 ---
  if (msg.pluginMessage.type === 'generate-xml') {
    try {
      discoveredImageNodes = [];
      processedComponentFamilyIds.clear();

      let xmlString = '<?xml version="1.0" encoding="UTF-8"?>\n';
      for (const node of selectedNodes) {
        xmlString += await nodeToXml(node, '', false);
      }

      mg.ui.postMessage({ pluginMessage:{ type: 'xml-generated', data: xmlString } });
      mg.ui.postMessage({ pluginMessage:{ type: 'images-discovered', count: discoveredImageNodes.length } });
      
      if (discoveredImageNodes.length > 0) {
        const filenameCounts = new Map<string, number>();
        discoveredImageNodes.forEach(imageNode => {
          const filename = generateImageFilename(imageNode);
          filenameCounts.set(filename, (filenameCounts.get(filename) || 0) + 1);
        });

        const duplicateFilenames = Array.from(filenameCounts.entries())
          .filter(([, count]) => count > 1)
          .map(([filename]) => filename);

        if (duplicateFilenames.length > 0) {
          mg.notify(`注意：发现同名图片，导出时可能被覆盖：${duplicateFilenames.join(', ')}`, { timeout: 8000 });
        } else {
          mg.notify(`已识别出 ${discoveredImageNodes.length} 张图片，现在可以导出了。`);
        }
      }

    } catch (error) {
      console.error('核心逻辑: 生成 XML 时发生错误:', error);
      mg.notify('生成 XML 失败，请查看插件开发者工具获取详情。');
    }
  }

  // --- 事件路由：处理“导出图片”请求 ---
  else if (msg.pluginMessage.type === 'export-images') {
    if (discoveredImageNodes.length === 0) {
      mg.notify('没有可导出的图片。请先点击“生成 XML”来查找图层中的图片。');
      mg.ui.postMessage({ pluginMessage:{ type: 'zip-and-download', payload: [] } });
      return;
    }

    try {
      const imageDataPayload = await Promise.all(discoveredImageNodes.map(async (imageNode) => {
        const imageBytes = await imageNode.exportAsync({ format: 'PNG' });
        const filename = generateImageFilename(imageNode);
        return { filename, data: imageBytes };
      }));
      mg.ui.postMessage({ pluginMessage:{ type: 'zip-and-download', payload: imageDataPayload } });

    } catch (error) {
      console.error('核心逻辑: 导出图片时发生错误:', error);
      mg.notify('导出图片失败，请查看插件开发者工具获取详情。');
    }
  }
  else if (msg.pluginMessage.type === 'notify') {
    mg.notify(msg.message, msg.options || {});
  }
};

/**
 * 将单个场景节点递归转换为XML字符串（异步版本）。
 */
async function nodeToXml(
  node: SceneNode,
  indent: string,
  isInsideFrame: boolean,
  parentInstanceInfo?: {
    offsetX: number;
    offsetY: number;
    reactions?: readonly Reaction[];
  }
): Promise<string> {
  
  // [修改 - 优先级1] 处理名称包含特殊关键字的 FRAME 节点。
  if (node.type === 'FRAME' && isInsideFrame) {
    const nodeNameLower = node.name.toLowerCase();
    let matchedKeyword: string | null = null;

    for (const keyword of SPECIAL_KEYWORD_NAMES) {
      if (nodeNameLower.includes(keyword)) {
        matchedKeyword = keyword;
        break; // 使用第一个匹配到的关键字
      }
    }

    // 如果找到了包含的关键字，则执行特殊逻辑
    if (matchedKeyword) {
      // 步骤 1: 遍历其子节点，使用带特殊跳过规则的函数收集图片资源。
      if ('children' in node && node.children) {
        for (const child of node.children) {
          const foundImages = collectImagesWithSpecialSkip(child);
          foundImages.forEach(img => {
            // 确保不重复添加。
            if (!discoveredImageNodes.includes(img)) {
              discoveredImageNodes.push(img);
            }
          });
        }
      }

      // 步骤 2: 生成此 FRAME 节点自身的 XML，并以匹配到的关键字作为标签名，不包含任何子节点 XML。
      const tagName = matchedKeyword.toUpperCase();
      const { offsetX = 0, offsetY = 0 } = parentInstanceInfo || {};
      const escapeXml = (str: string): string => str.replace(/[<>&'"]/g, c => ({'<':'&lt;','>':'&gt;','&':'&amp;',"'":'&apos;','"':'&quot;'})[c] || c);
      
      const attributes = [
        `x="${Math.round(node.x + offsetX)}"`,
        `y="${Math.round(node.y + offsetY)}"`,
        `w="${Math.round(node.width)}"`,
        `h="${Math.round(node.height)}"`
      ];

      const attrIndent = indent + '  ';
      const attributesString = `\n${attrIndent}${attributes.join('\n' + attrIndent)}`;
      
      // 生成一个没有子元素的XML标签。
      let xml = `${indent}<${tagName}${attributesString}>${escapeXml(node.name)}\n`;
      xml += `${indent}</${tagName}>\n`;
      
      return xml; // 处理完毕，直接返回
    }
  }
  
  if (node.type === 'INSTANCE' && node.mainComponent) {
    const mainComp = node.mainComponent;
    const componentParent = mainComp.parent;
    let componentFamilyId: string;
    let componentsToScan: readonly SceneNode[] = [];

    if (componentParent && componentParent.type === 'COMPONENT_SET') {
      componentFamilyId = componentParent.id;
      componentsToScan = componentParent.children;
    } else {
      componentFamilyId = mainComp.id;
      componentsToScan = [mainComp];
    }
    
    if (!processedComponentFamilyIds.has(componentFamilyId)) {
      processedComponentFamilyIds.add(componentFamilyId);
      componentsToScan.forEach(componentNode => {
        findImageNodesRecursive(componentNode).forEach(imageNode => {
          if (isImageRectangle(imageNode) && !discoveredImageNodes.includes(imageNode)) {
            discoveredImageNodes.push(imageNode);
          }
        });
      });
    }
    
    let childrenXml = '';
    if ('children' in node && node.children.length > 0) {
      const infoForChildren = {
        offsetX: (parentInstanceInfo?.offsetX || 0) + node.x,
        offsetY: (parentInstanceInfo?.offsetY || 0) + node.y,
        reactions: ('reactions' in node && node.reactions.length > 0) ? node.reactions : parentInstanceInfo?.reactions,
      };
      const childXmlPromises = node.children.map(child => nodeToXml(child, indent, isInsideFrame, infoForChildren));
      childrenXml = (await Promise.all(childXmlPromises)).join('');
    }
    return childrenXml;
  }

  const { offsetX = 0, offsetY = 0, reactions: inheritedReactions } = parentInstanceInfo || {};
  const escapeXml = (str: string): string => str.replace(/[<>&'"]/g, c => ({'<':'&lt;','>':'&gt;','&':'&amp;',"'":'&apos;','"':'&quot;'})[c] || c);
  const reactionsToProcess = inheritedReactions || ('reactions' in node ? node.reactions : []);
  const hasReaction = reactionsToProcess.length > 0;
  const isFillImage = isImageRectangle(node);
  const isBasicShape = isBasicVectorShape(node);

  let tagName: string;

  // 如果 tagName 未被上述特殊逻辑设置，则使用默认规则。
  if (!tagName) {
    if (isFillImage) tagName = 'IMAGE';
    else if (isBasicShape) tagName = hasReaction ? 'CANVAS' : 'IMAGE';
    else if (node.type === 'FRAME') tagName = isInsideFrame ? 'WIN' : 'SCREEN';
    else tagName = node.type;
  }
  
  const attributes = [
    `x="${Math.round(node.x + offsetX)}"`,
    `y="${Math.round(node.y + offsetY)}"`,
    `w="${Math.round(node.width)}"`,
    `h="${Math.round(node.height)}"`
  ];

  if (node.type === 'TEXT') {
    if (node.textStyles && node.textStyles.length > 0) {
      const firstStyle = node.textStyles[0];
      const fontName = firstStyle.textStyle.fontName;

      if (fontName) { 
          await mg.loadFontAsync(fontName);
          
          const processedFontName = fontName.family.replace(/ /g, '_') + '.bin';
          attributes.push(`fontName="${escapeXml(processedFontName)}"`);
      }

      if (firstStyle.textStyle.fontSize) {
          attributes.push(`fontSize="${firstStyle.textStyle.fontSize}"`);
      }

      if (Array.isArray(firstStyle.fills)) {
          const solidFill = firstStyle.fills.find(f => f.type === 'SOLID') as SolidPaint;
          if (solidFill) {
              attributes.push(`color="${rgbToHex(solidFill.color)}"`);
          }
      }
    }
    const contentWithPlaceholder = node.characters.replace(/\n/g, '__NL__');
    attributes.push(`content="${escapeXml(contentWithPlaceholder)}"`);

    const textAlignH = node.textAlignHorizontal;
    const textAlignV = node.textAlignVertical;
    var style = 'MID_CENTER';
    if (textAlignV === 'TOP') {
      style = textAlignH;
    }
    else if (textAlignV === 'CENTER') {
      style = 'MID_' + textAlignH;
    }
    attributes.push(`style="${style}"`);
  }

  if (tagName === 'IMAGE') {
    if (!discoveredImageNodes.includes(node)) {
      discoveredImageNodes.push(node);
    }
    attributes.push(`file="${node.name+'.bin'}"`);
    if (node.type === 'RECTANGLE' && node.blendMode === 'PASS_THROUGH') {
      attributes.push(`blendMode="imgSrcOverMode"`);
    }
  }

  const attrIndent = indent + '  ';
  let attributesString = `\n${attrIndent}${attributes.join('\n' + attrIndent)}`;
  let xml = `${indent}<${tagName}${attributesString}`;
  
  let reactionsContent = '';
  if (reactionsToProcess.length > 0) {
    reactionsToProcess.forEach(reaction => {
      const { action, trigger } = reaction;
      if (!trigger || !action || !action.destinationId) return;
      
      const reactionAttrList: string[] = [`trigger="${trigger.type}"`];
      const reactionAttrIndent = attrIndent + '  ';
      const reactionAttributesString = `\n${reactionAttrIndent}${reactionAttrList.join(`\n${reactionAttrIndent}`)}`;
      reactionsContent += `${attrIndent}<REACTION${reactionAttributesString}>\n${attrIndent}</REACTION>\n`;
    });
  }

  const hasChildren = 'children' in node && node.children.length > 0;
  const processChildren = hasChildren && node.type !== 'BOOLEAN_OPERATION';

  if (processChildren || reactionsContent) {
    xml += `>${escapeXml(node.name)}\n`;
    if (reactionsContent) xml += reactionsContent;
    if (processChildren) {
      const childIsInsideFrame = isInsideFrame || node.type === 'FRAME';
      const infoForChildren = {
        offsetX,
        offsetY,
        reactions: 'reactions' in node && node.reactions.length > 0 ? node.reactions : inheritedReactions
      };
      const childXmlPromises = node.children.map(child => nodeToXml(child, indent + '  ', childIsInsideFrame, infoForChildren));
      xml += (await Promise.all(childXmlPromises)).join('');
    }
    xml += `${indent}</${tagName}>\n`;
  } else {
    xml += `>${escapeXml(node.name)}\n${indent}</${tagName}>\n`;
  }

  return xml;
}

// --- 辅助函数区 ---

/**
 * [新函数] 递归查找图片节点，但会跳过任何名称在 SPECIAL_KEYWORD_NAMES 中的节点及其整个子树。
 * @param node 起始节点
 * @returns 返回找到的图片节点数组
 */
function collectImagesWithSpecialSkip(node: SceneNode): SceneNode[] {
  // 检查当前节点本身是否需要被跳过（名称是否完全匹配关键字）。
  if (SPECIAL_KEYWORD_NAMES.has(node.name.toLowerCase())) {
    return []; // 如果匹配，则不处理此节点及其任何子节点，直接返回空数组，实现“剪枝”。
  }

  let images: SceneNode[] = [];
  // 如果当前节点是图片，则收集它。
  if (isImageRectangle(node)) {
    images.push(node);
  }

  // 如果有子节点，则递归调用自身来处理子节点。
  if ('children' in node && node.children && node.type !== 'BOOLEAN_OPERATION') {
    for (const child of node.children) {
      images = images.concat(collectImagesWithSpecialSkip(child));
    }
  }
  return images;
}


function isBasicVectorShape(node: SceneNode): boolean {
  return ['RECTANGLE', 'LINE', 'ELLIPSE', 'POLYGON', 'STAR', 'PEN', 'BOOLEAN_OPERATION'].includes(node.type);
}

function rgbToHex(color: RGB): string {
  const toHex = (c: number) => Math.round(c * 255).toString(16).padStart(2, '0');
  return `#${toHex(color.r)}${toHex(color.g)}${toHex(color.b)}`;
}

function findImageNodesRecursive(node: SceneNode): SceneNode[] {
  let images: SceneNode[] = [];
  if (isImageRectangle(node)) images.push(node);
  if ('children' in node && node.type !== 'BOOLEAN_OPERATION') {
    node.children.forEach(child => {
      images = images.concat(findImageNodesRecursive(child));
    });
  }
  return images;
}

function generateImageFilename(node: SceneNode): string {
  return `${node.name.replace(/[^a-z0-9_.-]/gi, '_')}.png`;
}

function isImageRectangle(node: SceneNode): boolean {
  return node.type === 'RECTANGLE' && Array.isArray(node.fills) && node.fills.some(fill => fill.type === 'IMAGE');
}
