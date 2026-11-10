<template>
  <div class="container">
    <h2>设计转 XML 插件</h2>
    <p>1. 点击 "生成 XML"; 2. 如果有内容，"导出" 按钮将激活。</p>
    
    <p style="color: #FFC700; font-weight: bold;">注意：为确保兼容性，所有图层命名请勿使用中文字符。</p>
    
    <button @click="generateXml">生成 XML</button>
    
    <!-- [修改] 按钮文本、点击事件和禁用逻辑 -->
    <button @click="exportPackage" :disabled="!xmlOutput || xmlOutput === '正在生成...' || isExporting">
      {{ isExporting ? '正在导出...' : '导出 (.zip)' }}
    </button>
    
    <textarea ref="xmlTextarea" v-model="xmlOutput" placeholder="生成的 XML 将显示在这里..." readonly></textarea>
  </div>
</template>

<script setup lang="ts">
import { ref, onMounted, onUnmounted } from 'vue';
import JSZip from 'jszip';
import { saveAs } from 'file-saver';

const xmlTextarea = ref<HTMLTextAreaElement | null>(null);

const xmlOutput = ref('');
const isExporting = ref(false);
// [移除] 不再需要 canExportImages 状态
// const canExportImages = ref(false);

const generateXml = () => {
  xmlOutput.value = '正在生成...';
  // [移除] 不再需要重置 canExportImages
  // canExportImages.value = false;
  console.log('UI: 正在发送 "generate-xml" 消息到核心逻辑...');
  parent.postMessage({ pluginMessage: { type: 'generate-xml' } }, '*');
};

// [修改] 函数重命名，并发送新的消息类型
const exportPackage = () => {
  console.log('UI: 正在发送 "export-package" 消息到核心逻辑...');
  isExporting.value = true;
  parent.postMessage({ pluginMessage: { type: 'export-package' } }, '*');
};

const handleMessage = (event: MessageEvent) => {
  const message = event.data.pluginMessage;
  if (!message) return;

  console.log('UI: 收到来自核心逻辑的消息', message);

  if (message.type === 'xml-generated') {
    xmlOutput.value = message.data;
  }
  
  // [移除] 不再需要处理 images-discovered 消息
  // if (message.type === 'images-discovered') { ... }

  // [修改] 消息类型重命名，并更新打包逻辑
  if (message.type === 'package-for-download') {
    const payload = message.payload;
    isExporting.value = false;

    if (!payload || (!payload.images?.length && !payload.xml)) {
      parent.postMessage({ pluginMessage: { type: 'notify', message: '没有需要导出的内容。', options: { error: true } } }, '*');
      return;
    }

    const zip = new JSZip();
    
    // 1. 添加 XML 文件
    if (payload.xml) {
      zip.file("design.xml", payload.xml);
    }
    
    // 2. 添加图片文件
    if (Array.isArray(payload.images)) {
      payload.images.forEach((image: { filename: string, data: Uint8Array }) => {
        zip.file(`resource/${image.filename}`, image.data);
      });
    }

    zip.generateAsync({ type: "blob" })
      .then(content => {
        saveAs(content, "pixso_export.zip"); // 修改 zip 文件名
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
