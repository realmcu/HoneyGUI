// --- 类型定义 ---
/**
 * 定义从 UI 发送到核心逻辑的消息结构
 */
type PluginUIMessage = 
  | { type: 'generate-xml' }
  // [修改] 消息类型重命名
  | { type: 'export-package' } 
  | { type: 'notify', message: string, options?: NotificationOptions };

/**
 * 定义在递归时传递给子节点的父实例信息
 */
interface ParentInstanceInfo {
  offsetX?: number;
  offsetY?: number;
  reactions?: readonly Reaction[];
}

// --- 全局状态管理 ---
let discoveredImageNodes: SceneNode[] = [];
let processedComponentFamilyIds: Set<string> = new Set();
// [新增] 用于存储最近一次生成的 XML 字符串
let lastGeneratedXml: string = '';

const SPECIAL_KEYWORD_NAMES = new Set(['cellular', 'list', 'video']);

// 显示插件UI，并设置一个合适的初始尺寸
pixso.showUI(__html__, { width: 400, height: 600 });

/**
 * 主消息处理器，负责监听并响应来自UI界面的事件。
 */
pixso.ui.onmessage = async (msg: PluginUIMessage) => {
  console.log('核心逻辑: 收到来自 UI 的消息', msg);
  
  const messageData = msg;

  const currentPage = pixso.currentPage;
  const selectedNodes = currentPage.selection;

  if (selectedNodes.length === 0) {
    const notification = "请先在画板上选中至少一个图层。";
    pixso.notify(notification);
    if (messageData.type === 'generate-xml') {
      pixso.ui.postMessage({ type: 'xml-generated', data: '<!-- ' + notification + ' -->' });
    }
    return;
  }
  
  if (messageData.type === 'generate-xml') {
    try {
      const selectedFrames = selectedNodes.filter((node): node is FrameNode => node.type === 'FRAME');

      if (selectedFrames.length === 0) {
        const notification = "请选择一个或多个顶层画框 (Frame) 进行导出。";
        pixso.notify(notification);
        pixso.ui.postMessage({ type: 'xml-generated', data: `<!-- ${notification} -->` });
        return;
      }

      const notificationMessages: Set<string> = new Set();
      discoveredImageNodes = [];
      processedComponentFamilyIds.clear();
      lastGeneratedXml = ''; // 重置

      let xmlString = '<?xml version="1.0" encoding="UTF-8"?>\n';
      
      for (const frameNode of selectedFrames) {
        xmlString += await nodeToXml(frameNode, '', false, undefined, notificationMessages);
      }
      
      // [新增] 将生成的 XML 存储在全局变量中
      lastGeneratedXml = xmlString;

      pixso.ui.postMessage({ type: 'xml-generated', data: xmlString });
      
      // [移除] 不再需要向 UI 单独发送图片数量
      // pixso.ui.postMessage({ type: 'images-discovered', count: discoveredImageNodes.length });
      
      if (discoveredImageNodes.length > 0) {
        const filenameCounts = new Map<string, number>();
        discoveredImageNodes.forEach((imageNode: SceneNode) => {
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

      if (notificationMessages.size > 0) {
        const finalMessage = Array.from(notificationMessages).join('\n');
        pixso.notify(finalMessage, { timeout: 8000 });
      } else if (discoveredImageNodes.length > 0) {
        pixso.notify(`XML 生成完毕！已识别出 ${discoveredImageNodes.length} 张图片，现在可以导出了。`);
      } else {
        pixso.notify(`XML 生成完毕！现在可以导出了。`);
      }

    } catch (error) {
      console.error('核心逻辑: 生成 XML 时发生错误:', error);
      pixso.notify('生成 XML 失败，请查看开发者工具获取详情。');
    }
  }

  // [修改] 将 'export-images' 逻辑替换为 'export-package'
  else if (messageData.type === 'export-package') {
    if (!lastGeneratedXml) {
      pixso.notify('没有可导出的内容。请先点击“生成 XML”。');
      pixso.ui.postMessage({ type: 'package-for-download', payload: null });
      return;
    }

    try {
      // 1. 异步导出所有图片
      const imageDataPayload = await Promise.all(discoveredImageNodes.map(async (imageNode) => {
        const imageBytes = await imageNode.exportAsync({ format: 'PNG' });
        const filename = generateImageFilename(imageNode);
        return { filename, data: imageBytes };
      }));

      // 2. 准备包含 XML 和图片数据的 payload
      const payload = {
        xml: lastGeneratedXml,
        images: imageDataPayload
      };
      
      // 3. 将所有数据一次性发送到 UI 进行打包
      pixso.ui.postMessage({ type: 'package-for-download', payload: payload });

    } catch (error) {
      console.error('核心逻辑: 导出打包文件时发生错误:', error);
      pixso.notify('导出失败，请查看插件开发者工具获取详情。');
    }
  }

  else if (messageData.type === 'notify') {
    pixso.notify(messageData.message, messageData.options || {});
  }
};

// ... nodeToXml 和其他辅助函数保持不变 ...
// (此处省略了未修改的 nodeToXml 及其他辅助函数代码，它们与您之前提供的版本相同)
async function nodeToXml(
  node: SceneNode, 
  indent: string, 
  isInsideFrame: boolean, 
  parentInstanceInfo: ParentInstanceInfo | undefined, 
  notificationMessages: Set<string>
): Promise<string> {
//highlight-start
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
            if (!discoveredImageNodes.some(n => n.id === img.id)) {
              discoveredImageNodes.push(img);
            }
          });
        }
      }

      const tagName = matchedKeyword.toUpperCase();
      const { offsetX = 0, offsetY = 0 } = parentInstanceInfo || {};
      const escapeXml = (str: string) => str.replace(/[<>&'"]/g, c => ({'<':'&lt;','>':'&gt;','&':'&amp;',"'":'&apos;','"':'&quot;'})[c as keyof typeof escapeMap] || c);
      const escapeMap = {'<':'&lt;','>':'&gt;','&':'&amp;',"'":'&apos;','"':'&quot;'};
      
      const attributes = [
        `x="${Math.round(node.x + offsetX)}"`,
        `y="${Math.round(node.y + offsetY)}"`,
        `w="${Math.round(node.width)}"`,
        `h="${Math.round(node.height)}"`,
        `hide="${node.visible === false ? 1 : 0}"`,
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

    // 1. 确定要扫描的组件范围（是单个主组件还是整个组件集）
    if (componentParent && componentParent.type === 'COMPONENT_SET') {
      componentFamilyId = componentParent.id;
      componentsToScan = componentParent.children; // 扫描组件集下的所有变体
    } else {
      componentFamilyId = mainComp.id;
      componentsToScan = [mainComp]; // 扫描单个主组件
    }
    
    // 2. 检查这个组件族是否已经被处理过，防止重复工作
    if (!processedComponentFamilyIds.has(componentFamilyId)) {
      processedComponentFamilyIds.add(componentFamilyId);

      // 3. 遍历所有需要扫描的组件定义（变体）
      for (const componentNode of componentsToScan) {
        // 4. 在每个组件定义内部递归查找所有符合条件的图片节点
        const imagesInComponent = findImageNodesRecursive(componentNode);
        
        for (const imageNode of imagesInComponent) {
          // 5. 检查全局列表，如果图片未被添加，则添加
          const isAlreadyDiscovered = discoveredImageNodes.some(n => n.id === imageNode.id);
          if (!isAlreadyDiscovered) {
            discoveredImageNodes.push(imageNode);
          }
        }
      }
    }
    
    let childrenXml = '';
    if ('children' in node && node.children.length > 0) {
      const infoForChildren: ParentInstanceInfo = {
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
  const escapeXml = (str: string) => str.replace(/[<>&'"]/g, c => ({'<':'&lt;','>':'&gt;','&':'&amp;',"'":'&apos;','"':'&quot;'})[c as keyof typeof escapeMap] || c);
  const escapeMap = {'<':'&lt;','>':'&gt;','&':'&amp;',"'":'&apos;','"':'&quot;'};
  const reactionsToProcess = inheritedReactions || ('reactions' in node ? node.reactions : []);
  const hasReaction = reactionsToProcess.length > 0;
  const isFillImage = isImageRectangle(node);
  const isBasicShape = isBasicVectorShape(node);

  let tagName: string;

  if (isFillImage) tagName = 'IMAGE';
  else if (isBasicShape) tagName = hasReaction ? 'CANVAS' : 'IMAGE';
  else if (node.type === 'FRAME') tagName = isInsideFrame ? 'WIN' : 'SCREEN';
  else tagName = node.type;
  
  const attributes = [
    `x="${Math.round(node.x + offsetX)}"`,
    `y="${Math.round(node.y + offsetY)}"`,
    `w="${Math.round(node.width)}"`,
    `h="${Math.round(node.height)}"`,
    `hide="${node.visible === false ? 1 : 0}"`,
  ];

  if (node.type === 'TEXT') {
    if (node.fontName) { 
        await pixso.loadFontAsync(node.fontName);
        const processedFontName = (node.fontName as FontName).family.replace(/ /g, '_') + '.bin';
        attributes.push(`fontName="${escapeXml(processedFontName)}"`);
    }
    if (node.fontSize) {
        attributes.push(`fontSize="${node.fontSize as number}"`);
    }
    if (Array.isArray(node.fills) && node.fills[0]?.type === 'SOLID') {
        attributes.push(`color="${rgbToHex(node.fills[0].color)}"`);
    }
    const contentWithPlaceholder = node.characters.replace(/\n/g, '__NL__');
    attributes.push(`content="${escapeXml(contentWithPlaceholder)}"`);
    let style = 'MID_CENTER';
    if (node.textAlignVertical === 'TOP') style = node.textAlignHorizontal;
    else if (node.textAlignVertical === 'CENTER') style = 'MID_' + node.textAlignHorizontal;
    attributes.push(`style="${style}"`);
  }

  if (tagName === 'IMAGE') {
    if (!discoveredImageNodes.some(n => n.id === node.id)) {
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
    //highlight-start
    reactionsToProcess.forEach((reaction: Reaction) => {
    //highlight-end
      const action = reaction.action as Action;
      const trigger = reaction.trigger as Trigger;
      if (!trigger || !action || !action.destinationId) return;
      const reactionAttrList = [`trigger="${trigger.type}"`];
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
      const infoForChildren: ParentInstanceInfo = {
        offsetX,
        offsetY,
        reactions: ('reactions' in node && node.reactions.length > 0) ? node.reactions : inheritedReactions
      };
      const childXmlPromises = (node.children as SceneNode[]).map(child => nodeToXml(child, indent + '  ', childIsInsideFrame, infoForChildren, notificationMessages));
      xml += (await Promise.all(childXmlPromises)).join('');
    }
    xml += `${indent}</${tagName}>\n`;
  } else {
    xml += `>${escapeXml(processedNodeName)}\n${indent}</${tagName}>\n`;
  }
  return xml;
}

// --- 辅助函数区 ---

//highlight-start
function collectImagesWithSpecialSkip(node: SceneNode): SceneNode[] {
//highlight-end
    if (Array.from(SPECIAL_KEYWORD_NAMES).some(keyword => node.name.toLowerCase().includes(keyword))) {
        return [];
    }
    let images: SceneNode[] = [];
    const isAlreadyDiscovered = discoveredImageNodes.some(n => n.id === node.id);
    if (!isAlreadyDiscovered && (isImageRectangle(node) || isBasicVectorShape(node))) {
        images.push(node);
    }
    else if (node.type === 'INSTANCE' && node.mainComponent) {
      const mainComp = node.mainComponent;
      const componentParent = mainComp.parent;
      let componentFamilyId: string;
      let componentsToScan: readonly SceneNode[] = [];
  
      // 1. 确定要扫描的组件范围（是单个主组件还是整个组件集）
      if (componentParent && componentParent.type === 'COMPONENT_SET') {
        componentFamilyId = componentParent.id;
        componentsToScan = componentParent.children; // 扫描组件集下的所有变体
      } else {
        componentFamilyId = mainComp.id;
        componentsToScan = [mainComp]; // 扫描单个主组件
      }
      
      // 2. 检查这个组件族是否已经被处理过，防止重复工作
      if (!processedComponentFamilyIds.has(componentFamilyId)) {
        processedComponentFamilyIds.add(componentFamilyId);
  
        // 3. 遍历所有需要扫描的组件定义（变体）
        for (const componentNode of componentsToScan) {
          // 4. 在每个组件定义内部递归查找所有符合条件的图片节点
          const imagesInComponent = findImageNodesRecursive(componentNode);
          
          for (const imageNode of imagesInComponent) {
            // 5. 检查全局列表，如果图片未被添加，则添加
            const isAlreadyDiscovered = discoveredImageNodes.some(n => n.id === imageNode.id);
            if (!isAlreadyDiscovered) {
              discoveredImageNodes.push(imageNode);
            }
          }
        }
      }
      return images;
    }
    if ('children' in node && node.children && node.type !== 'BOOLEAN_OPERATION' && node.type !== 'GROUP') {
      for (const child of node.children) {
        images = images.concat(collectImagesWithSpecialSkip(child));
      }
    }
    return images;
}

//highlight-start
function isBasicVectorShape(node: SceneNode): boolean {
    return ['RECTANGLE', 'LINE', 'ELLIPSE', 'POLYGON', 'STAR', 'VECTOR', 'BOOLEAN_OPERATION', 'GROUP'].includes(node.type);
}

function rgbToHex(color: RGB): string {
    const toHex = (c: number) => Math.round(c * 255).toString(16).padStart(2, '0');
    return `#${toHex(color.r)}${toHex(color.g)}${toHex(color.b)}`;
}

function findImageNodesRecursive(node: SceneNode): SceneNode[] {
    let images: SceneNode[] = [];
    if (isImageRectangle(node) || isBasicVectorShape(node)) {
        images.push(node);
    }
    if ('children' in node && node.type !== 'BOOLEAN_OPERATION' && node.type !== 'GROUP') {
        for(const child of node.children){
             images = images.concat(findImageNodesRecursive(child));
        }
    }
    return images;
}

//highlight-start
function generateImageFilename(node: SceneNode): string {
    return `${node.name.replace(/[^a-z0-9_.-]/gi, '_')}.png`;
}

function isImageRectangle(node: SceneNode): node is RectangleNode {
//highlight-end
    return node.type === 'RECTANGLE' && Array.isArray(node.fills) && node.fills.some(fill => fill.type === 'IMAGE');
}
