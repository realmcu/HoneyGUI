<template>
  <div class="container">
    <h2>设计转 XML 插件</h2>
    <p>1. 点击 "生成 XML"; 2. 如果有图片，"导出图片" 按钮将激活。</p>
    
    <button @click="generateXml">生成 XML</button>
    
    <button @click="exportImages" :disabled="!canExportImages || isExporting">
      {{ isExporting ? '正在导出...' : '导出图片 (.zip)' }}
    </button>
    
    <!-- [修改] 添加 ref="xmlTextarea" 以便在脚本中访问它 -->
    <textarea ref="xmlTextarea" v-model="xmlOutput" placeholder="生成的 XML 将显示在这里..." readonly></textarea>
    
    <button @click="copyToClipboard" :disabled="!xmlOutput">复制到剪贴板</button>
  </div>
</template>

<script setup lang="ts">
import { ref, onMounted, onUnmounted } from 'vue';
import JSZip from 'jszip';
import { saveAs } from 'file-saver';

// [新增] 创建一个 ref 来引用 textarea 元素
const xmlTextarea = ref<HTMLTextAreaElement | null>(null);

const xmlOutput = ref('');
const isExporting = ref(false);
const canExportImages = ref(false);

const generateXml = () => {
  xmlOutput.value = '正在生成...';
  canExportImages.value = false;
  console.log('UI: 正在发送 "generate-xml" 消息到核心逻辑...');
  // [修改] 统一使用 MasterGo 推荐的消息格式
  parent.postMessage({ pluginMessage: { type: 'generate-xml' } }, '*');
};

const exportImages = () => {
  console.log('UI: 正在发送 "export-images" 消息到核心逻辑...');
  isExporting.value = true;
  // [修改] 统一使用 MasterGo 推荐的消息格式
  parent.postMessage({ pluginMessage: { type: 'export-images' } }, '*');
};

/**
 * --- [核心修改] ---
 * 点击按钮时，全选文本并提示用户手动复制。
 */
const copyToClipboard = () => {
  if (!xmlOutput.value || !xmlTextarea.value) return;

  // 1. 让文本框获得焦点并全选内部文本
  xmlTextarea.value.focus();
  xmlTextarea.value.select();

  // 2. 发送消息到主线程，请求显示一个提示
  parent.postMessage({ 
    pluginMessage: { 
      type: 'notify', 
      message: '已全选，请按 Ctrl+C 或 Cmd+C 复制' 
    } 
  }, '*');
};

const handleMessage = (event: MessageEvent) => {
  // [修改] MasterGo 插件消息封装在 event.data.pluginMessage 中
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
    isExporting.value = false; // 无论结果如何，都重置导出状态

    if (!imageData || imageData.length === 0) {
      // [修改] 使用 mg.notify 替代 alert，体验更佳
      parent.postMessage({ pluginMessage: { type: 'notify', message: '没有需要导出的图片。', options: { error: true } } }, '*');
      return;
    }

    const zip = new JSZip();
    // 确保 imageData 是一个数组
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
.container { padding: 16px; display: flex; flex-direction: column; gap: 12px; }
h2 { margin: 0; color: white; }
p { font-size: 12px; color: #666; margin-top: 0; }
button { padding: 8px 12px; border: none; background-color: #007aff; color: white; border-radius: 4px; cursor: pointer; transition: background-color 0.2s; }
button:hover { background-color: #0056b3; }
button:disabled { background-color: #ccc; cursor: not-allowed; }
textarea { width: calc(100% - 8px); box-sizing: border-box; min-height: 200px; font-family: monospace; font-size: 12px; border: 1px solid #ddd; border-radius: 4px; resize: vertical; }
</style>
