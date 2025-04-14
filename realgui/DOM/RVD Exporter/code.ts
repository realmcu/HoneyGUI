// code.ts
async function generateXMLAndImages(): Promise<{ xml: string, images: { hash: string, data: Uint8Array }[], projectName: string }> {
  const page = figma.currentPage;
  const frames = page.findAll(node => node.type === "FRAME") as FrameNode[];
  const imageSet = new Set<string>();
  const projectName = figma.root.name;

  let xml = `<?xml version="1.0" encoding="utf-8"?>\n<app>${projectName}\n`;
  xml += `    <script file="app/${projectName}/${projectName}.js"></script>\n`;
  xml += `    <screen w="198" h="242" x="0" y="0">screen0\n`;
  xml += `        <tabview transition="smart_animate" gestures="true" w="198" h="242" x="0" y="0">tabview0\n`;

  frames.forEach((frame: FrameNode, index: number) => {
    xml += `            <tab idx="${index}" idy="0">tab${index}\n`;

    if (frame.reactions && frame.reactions.length > 0) {
      const reaction = frame.reactions[0];
      if (
        reaction.trigger?.type === "ON_CLICK" &&
        reaction.action?.type === "NODE" &&
        "destinationId" in reaction.action
      ) {
        const destId = reaction.action.destinationId;
        const destIndex = frames.findIndex(f => f.id === destId);
        if (destIndex !== -1) {
          xml += `                <win w="${frame.width}" h="${frame.height}" x="0" y="0">win0\n`;
          xml += `                    <onClick type="jump" to="tabview" id="${destIndex},0,tabview0"></onClick>\n`;
          xml += `                </win>\n`;
        }
      }
    }

    // Handle rectangles with image fills
    const rectangles = frame.findAll((node: SceneNode) => node.type === "RECTANGLE") as RectangleNode[];
    rectangles.forEach((rect: RectangleNode) => {
      const fills = rect.fills as Paint[];
      const fill = fills.length > 0 ? fills[0] : undefined;
      if (fill && fill.type === "IMAGE" && fill.imageHash) {
        imageSet.add(fill.imageHash);
        xml += `                <img\n`;
        xml += `                    opacity="255"\n`;
        xml += `                    blendMode="imgBypassMode"\n`;
        xml += `                    file="resource/${fill.imageHash}.bin"\n`;
        xml += `                    rotationAngle="${rect.rotation}"\n`;
        xml += `                    scaleX="${fill.scalingFactor || 1}"\n`;
        xml += `                    scaleY="${fill.scalingFactor || 1}"\n`;
        xml += `                    w="${rect.width}"\n`;
        xml += `                    h="${rect.height}"\n`;
        xml += `                    x="${rect.x}"\n`;
        xml += `                    y="${rect.y}">${fill.imageHash}\n`;
        xml += `                </img>\n`;
      }
    });

    // Handle text nodes
    const texts = frame.findAll((node: SceneNode) => node.type === "TEXT") as TextNode[];
    texts.forEach((text: TextNode, textIndex: number) => {
      const fill = Array.isArray(text.fills) && text.fills.length > 0 ? text.fills[0] : null;
      let color = "#ffffffff"; // Default to white with full opacity
      if (fill && fill.type === "SOLID") {
        const r = Math.round(fill.color.r * 255);
        const g = Math.round(fill.color.g * 255);
        const b = Math.round(fill.color.b * 255);
        const a = Math.round((fill.opacity || 1) * 255);
        color = `#${a.toString(16).padStart(2, "0")}${r.toString(16).padStart(2, "0")}${g.toString(16).padStart(2, "0")}${b.toString(16).padStart(2, "0")}`;
      }
      const fontSize = text.fontSize !== figma.mixed ? text.fontSize : 32;
      // Get font family, default to "Inter" if mixed
      const fontFamily = text.fontName !== figma.mixed ? text.fontName.family : "Inter";
      // Sanitize font family for filename (replace spaces with underscores, remove invalid chars)
      const sanitizedFontFamily = fontFamily.replace(/\s+/g, "_").replace(/[^a-zA-Z0-9_-]/g, "");
      const fontPath = `app/${projectName}/resource/font/${sanitizedFontFamily}.ttf`;
      xml += `                <textbox\n`;
      xml += `                    text="${text.characters.replace(/"/g, "&quot;")}"\n`; // Proper XML escaping
      xml += `                    font="${fontPath}"\n`;
      xml += `                    fontSize="${fontSize}"\n`;
      xml += `                    color="${color}"\n`;
      xml += `                    mode="truncate"\n`;
      xml += `                    inputable="0"\n`;
      xml += `                    w="${text.width}"\n`;
      xml += `                    h="${text.height}"\n`;
      xml += `                    x="${text.x}"\n`;
      xml += `                    y="${text.y}">text${textIndex}\n`;
      xml += `                </textbox>\n`;
    });

    xml += `            </tab>\n`;
  });

  xml += `        </tabview>\n`;
  xml += `    </screen>\n`;
  xml += `</app>`;

  const images = await Promise.all(
    Array.from(imageSet).map(async (hash) => {
      const image = figma.getImageByHash(hash);
      if (image) {
        const data = await image.getBytesAsync();
        return { hash, data };
      }
      return null;
    })
  ).then(results => results.filter((img): img is { hash: string, data: Uint8Array } => img !== null));

  return { xml, images, projectName };
}

// Show fixed-size UI
figma.showUI(__html__, { width: 400, height: 400, title: "XML Preview" });

// Generate and send XML + images + project name to UI
generateXMLAndImages().then(({ xml, images, projectName }) => {
  figma.ui.postMessage({ xml, images, projectName });
});

// Handle UI messages for notifications
figma.ui.onmessage = (msg) => {
  if (msg.type === "notify") {
    figma.notify(msg.text);
  }
  else if (msg.type === "close") {
    console.log("RVD-Exporter closed");
    figma.closePlugin();
  }
};