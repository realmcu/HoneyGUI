import { UIMessage } from '@messages/sender'

// --- 全局状态管理 ---
let discoveredImageNodes: SceneNode[] = [];
let processedComponentFamilyIds = new Set<string>();
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
      // [新需求] 过滤用户的选择，只保留顶层的 FRAME 节点进行处理
      const selectedFrames = selectedNodes.filter(node => node.type === 'FRAME');

      // [新需求] 如果用户有选择，但选择的都不是 FRAME，则提示用户
      if (selectedFrames.length === 0) {
        const notification = "请选择一个或多个顶层画框 (Frame) 进行导出。";
        mg.notify(notification);
        mg.ui.postMessage({ pluginMessage: { type: 'xml-generated', data: `<!-- ${notification} -->` } });
        return;
      }

      const notificationMessages = new Set<string>();

      discoveredImageNodes = [];
      processedComponentFamilyIds.clear();

      let xmlString = '<?xml version="1.0" encoding="UTF-8"?>\n';
      
      // [修改] 遍历过滤后的 FRAME 列表，而不是原始选择列表
      for (const frameNode of selectedFrames) {
        // 每次调用都会将一个完整的 <SCREEN>...</SCREEN> 块追加到 xmlString 中
        // 同时，discoveredImageNodes 和 notificationMessages 会在多次调用中累积结果
        xmlString += await nodeToXml(frameNode, '', false, undefined, notificationMessages);
      }

      mg.ui.postMessage({ pluginMessage:{ type: 'xml-generated', data: xmlString } });
      mg.ui.postMessage({ pluginMessage:{ type: 'images-discovered', count: discoveredImageNodes.length } });
      
      // 检查图片重名问题，并将警告信息添加到集合中
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
          notificationMessages.add(`图片重名警告: ${duplicateFilenames.join(', ')} (导出时可能被覆盖)`);
        }
      }

      // 在所有流程结束后，统一处理并显示通知
      if (notificationMessages.size > 0) {
        const finalMessage = Array.from(notificationMessages).join('\n');
        mg.notify(finalMessage, { timeout: 8000});
      } else if (discoveredImageNodes.length > 0) {
        mg.notify(`已识别出 ${discoveredImageNodes.length} 张图片，现在可以导出了。`);
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
 * [无需修改] 此函数的设计已经能够正确处理作为根节点传入的 FRAME。
 */
async function nodeToXml(
  node: SceneNode,
  indent: string,
  isInsideFrame: boolean,
  parentInstanceInfo?: {
    offsetX: number;
    offsetY: number;
    reactions?: readonly Reaction[];
  },
  notificationMessages?: Set<string>
): Promise<string> {
  
  const chineseCharsRegex = /[\u4e00-\u9fa5\u3000-\u303f\uff00-\uffef]/;

  if (node.type === 'FRAME' && isInsideFrame) {
    const nodeNameLower = node.name.toLowerCase();
    let matchedKeyword: string | null = null;

    for (const keyword of SPECIAL_KEYWORD_NAMES) {
      if (nodeNameLower.includes(keyword)) {
        matchedKeyword = keyword;
        break;
      }
    }

    if (matchedKeyword) {
      if ('children' in node && node.children) {
        for (const child of node.children) {
          const foundImages = collectImagesWithSpecialSkip(child);
          foundImages.forEach(img => {
            if (!discoveredImageNodes.includes(img)) {
              discoveredImageNodes.push(img);
            }
          });
        }
      }

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
      
      let processedNodeName = node.name;
      if (chineseCharsRegex.test(node.name) && notificationMessages) {
        processedNodeName = 'widget';
        notificationMessages.add(`图层名含中文字符: '${node.name}' 请重命名`);
      }

      let xml = `${indent}<${tagName}${attributesString}>${escapeXml(processedNodeName)}\n`;
      xml += `${indent}</${tagName}>\n`;
      
      return xml;
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
      const childXmlPromises = node.children.map(child => nodeToXml(child, indent, isInsideFrame, infoForChildren, notificationMessages));
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

  if (isFillImage) tagName = 'IMAGE';
  else if (isBasicShape) tagName = hasReaction ? 'CANVAS' : 'IMAGE';
  // [关键逻辑点] 当一个顶层 FRAME (isInsideFrame=false) 传入时，这里会正确地将其 tagName 设置为 'SCREEN'
  else if (node.type === 'FRAME') tagName = isInsideFrame ? 'WIN' : 'SCREEN';
  else tagName = node.type;
  
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
  const processChildren = hasChildren && node.type !== 'BOOLEAN_OPERATION' && node.type !== 'GROUP';

  let processedNodeName = node.name;
  if (chineseCharsRegex.test(node.name) && notificationMessages) {
    processedNodeName = 'widget';
    notificationMessages.add(`图层名含中文字符: '${node.name}' 请重命名`);
  }

  if (processChildren || reactionsContent) {
    xml += `>${escapeXml(processedNodeName)}\n`;
    if (reactionsContent) xml += reactionsContent;
    if (processChildren) {
      const childIsInsideFrame = isInsideFrame || node.type === 'FRAME';
      const infoForChildren = {
        offsetX,
        offsetY,
        reactions: 'reactions' in node && node.reactions.length > 0 ? node.reactions : inheritedReactions
      };
      const childXmlPromises = node.children.map(child => nodeToXml(child, indent + '  ', childIsInsideFrame, infoForChildren, notificationMessages));
      xml += (await Promise.all(childXmlPromises)).join('');
    }
    xml += `${indent}</${tagName}>\n`;
  } else {
    xml += `>${escapeXml(processedNodeName)}\n${indent}</${tagName}>\n`;
  }

  return xml;
}

// --- 辅助函数区 (保持不变) ---

function collectImagesWithSpecialSkip(node: SceneNode): SceneNode[] {
  if (SPECIAL_KEYWORD_NAMES.has(node.name.toLowerCase())) {
    return [];
  }

  let images: SceneNode[] = [];
  if (isImageRectangle(node)) {
    images.push(node);
  }

  if ('children' in node && node.children && node.type !== 'BOOLEAN_OPERATION') {
    for (const child of node.children) {
      images = images.concat(collectImagesWithSpecialSkip(child));
    }
  }
  return images;
}


function isBasicVectorShape(node: SceneNode): boolean {
  return ['RECTANGLE', 'LINE', 'ELLIPSE', 'POLYGON', 'STAR', 'PEN', 'BOOLEAN_OPERATION', 'GROUP'].includes(node.type);
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
