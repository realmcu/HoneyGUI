<template>
  <div class="container">
    <h2>设计转 XML 插件</h2>
    <p>1. 点击 "生成 XML"; 2. 如果有图片，"导出图片" 按钮将激活。</p>
    
    <!-- [新增] 添加关于图层命名的提示 -->
    <p style="color: #FFC700; font-weight: bold;">注意：为确保兼容性，所有图层命名请勿使用中文字符。</p>
    
    <button @click="generateXml">生成 XML</button>
    
    <button @click="exportImages" :disabled="!canExportImages || isExporting">
      {{ isExporting ? '正在导出...' : '导出图片 (.zip)' }}
    </button>
    
    <textarea ref="xmlTextarea" v-model="xmlOutput" placeholder="生成的 XML 将显示在这里..." readonly></textarea>
    
    <button @click="copyToClipboard" :disabled="!xmlOutput">复制到剪贴板</button>
  </div>
</template>

<script setup lang="ts">
import { ref, onMounted, onUnmounted } from 'vue';
import JSZip from 'jszip';
import { saveAs } from 'file-saver';

const xmlTextarea = ref<HTMLTextAreaElement | null>(null);

const xmlOutput = ref('');
const isExporting = ref(false);
const canExportImages = ref(false);

const generateXml = () => {
  xmlOutput.value = '正在生成...';
  canExportImages.value = false;
  console.log('UI: 正在发送 "generate-xml" 消息到核心逻辑...');
  parent.postMessage({ pluginMessage: { type: 'generate-xml' } }, '*');
};

const exportImages = () => {
  console.log('UI: 正在发送 "export-images" 消息到核心逻辑...');
  isExporting.value = true;
  parent.postMessage({ pluginMessage: { type: 'export-images' } }, '*');
};

const copyToClipboard = () => {
  if (!xmlOutput.value || !xmlTextarea.value) return;

  xmlTextarea.value.focus();
  xmlTextarea.value.select();

  parent.postMessage({ 
    pluginMessage: { 
      type: 'notify', 
      message: '已全选，请按 Ctrl+C 或 Cmd+C 复制' 
    } 
  }, '*');
};

const handleMessage = (event: MessageEvent) => {
  const message = event.data.pluginMessage;
  if (!message) return;

  console.log('UI: 收到来自核心逻辑的消息', message);

  if (message.type === 'xml-generated') {
    xmlOutput.value = message.data;
  }
  
  if (message.type === 'images-discovered') {
    if (message.count > 0) {
      canExportImages.value = true;
    }
  }

  if (message.type === 'zip-and-download') {
    const imageData = message.payload;
    isExporting.value = false;

    if (!imageData || imageData.length === 0) {
      parent.postMessage({ pluginMessage: { type: 'notify', message: '没有需要导出的图片。', options: { error: true } } }, '*');
      return;
    }

    const zip = new JSZip();
    if (Array.isArray(imageData)) {
      imageData.forEach((image: { filename: string, data: Uint8Array }) => {
        zip.file(image.filename, image.data);
      });
    }

    zip.generateAsync({ type: "blob" })
      .then(content => {
        saveAs(content, "exported_images.zip");
      });
  }
};

onMounted(() => {
  window.addEventListener('message', handleMessage);
});

onUnmounted(() => {
  window.removeEventListener('message', handleMessage);
});
</script>

<style>
/* 样式保持不变 */
.container {background-color: #2C2C2C; padding: 16px; display: flex; flex-direction: column; gap: 12px; }
h2 { margin: 0; color: white; }
p { font-size: 12px; color: #D8D8D8; margin-top: 0; }
button { padding: 8px 12px; border: none; background-color: #007aff; color: white; border-radius: 4px; cursor: pointer; transition: background-color 0.2s; }
button:hover { background-color: #0056b3; }
button:disabled { background-color: #ccc; cursor: not-allowed; }
textarea { width: calc(100% - 8px); box-sizing: border-box; min-height: 200px; font-family: monospace; font-size: 12px; border: 1px solid #ddd; border-radius: 4px; resize: vertical; }
</style>
