# MasterGo D2XML Plugin
This is a plugin for MasterGo, designed to streamline the workflow for designers and developers by converting design layouts into a specific XML format and exporting all associated image assets.

Built upon the official MasterGo Vue template, this plugin parses the properties of layers—such as position, size, text styles, and image fills—to automatically generate well-structured XML code suitable for custom UI frameworks.

## Key Features
1. XML Generation: Creates a structured XML string from any selected layer, typically a top-level Frame.
2. Asset Discovery: Automatically scans the selected layer hierarchy for any layers with image fills and adds them to an export queue.
3. Image Export: Exports all discovered images as a single .zip file with one click.
4. Component Parsing: Correctly handles Component Instances and resolves their child layers.
5. Text Processing: Extracts text content, font family, font size, and color attributes.
6. Special Keyword Handling: Applies custom logic to nested Frames whose names contain special keywords (e.g., list), generating unique XML tags (e.g., <LIST>) and employing a specialized asset collection strategy.
7. Precautions: A list of important notes is provided to prevent discrepancies between the exported XML/assets and the UI design.


## Development Environment Setup
### Prerequisites
1. Node.js: Required to run Yarn and the project's build scripts. We recommend the latest LTS version. You can download it from [Node.js]("url "https://nodejs.org/zh-cn")(LTS version recommended).

2. This project uses the Yarn package manager. If you have a recent version of Node.js (v16.10+), you can enable it by running:
    ```bash
      corepack enable
    ```
    If you are using an older version of Node.js, you can install Yarn globally using npm (which is included with Node.js):
    ```bash
      npm install -g yarn
    ```

3. MasterGo Desktop Client: The plugin can only be loaded and tested within the MasterGo desktop application.

### Installation and Build
1. Install Dependencies
Once inside the project directory, run the following command to install all the required dependencies listed in the package.json file.
    ```bash
      yarn install
    ```
    This will create a node_modules folder containing all the necessary packages.
2. Build the Plugin
  This command will generate a production-ready dist folder containing the compiled plugin files:
    ```bash
      yarn build
    ```
    Each time relevant files (main.ts, app.vue) are modified, a rebuild is required to get the regenerated `dist/main.js` file.



### Loading the Plugin in MasterGo
1. Open the MasterGo desktop client.
2. Navigate to the main menu: Plugins > Development > Import plugin from manifest....
3. In the file dialog that opens, navigate to your project directory and select the manifest.json file.
4. Once imported successfully, the plugin will appear under the Plugins > Development menu, ready to be run and tested.


## User Guide
Follow these steps to export your designs using the plugin.

### Step 1: Open the Plugin
In the MasterGo app, open the plugin by navigating to Plugins > Development > D2XML.

### Step 2: Select a Master Layer
On your canvas, select all the top-level Frames you wish to export. The plugin requires a selection to function. This Frame typically represents an entire screen or a complete UI view.

### Step 3: Generate XML
Click the "Generate XML" button in the plugin panel.
The plugin will process your selection and display the generated XML code in the text area.
A notification will appear, informing you of the number of images discovered within the selected layers.

### Step 4: Export Images
- After the XML has been generated, the "Export Images" button will become active if any images were found.
- Click it to bundle all discovered images into a single .zip file, which will then be downloaded by your browser.
- Note: You must run "Generate XML" first to discover images before you can export them.

### Step 5: Splitting the XML File for HoneyGUI
- After generating the XML, copy the text content into the `tool/D2XML/spilt_xml/__template.xml` file and save it.
- Double-click the `split_xml.bat` script file in the same directory to generate multiple XML files. Copy these files to the HoneyGUI project folder, which is typically `root/root_image/`.


## Core Concept: Special Keyword Handling
To support dynamic layouts like lists and grids, the plugin uses a rule-based system that activates when layers are named with specific keywords.

### Trigger Condition
To create widgets not supported by MasterGo, this special logic is triggered when a nested Frame (a Frame that is inside another Frame) has a name that contains one of the following keywords (case-insensitive):
-   `cellular`
-   `list`
-   `video`

Important: This rule does not apply to the top-level Frame you initially select on the canvas.

### Effects
If a nested Frame named my_contact_list is processed, the following occurs:

1. XML Tag Is Changed: In the output XML, the Frame is converted to a tag matching the keyword, not a generic <WIN>.
2. Child Nodes Are Not Rendered in XML: All layers inside my_contact_list will not be converted into XML elements.
3. Specialized Image Collection: The plugin will still search inside the my_contact_list Frame to find images. However, if it encounters a descendant layer whose name exactly matches a keyword (e.g., a layer named precisely list), that layer and its entire subtree will be skipped.


## Precautions
- To ensure compatibility, please do not use Chinese characters in layer names.
- When composing a single image from multiple images (including vector graphics), please create a `<GROUP>` for these layers. This ensures that only one `<IMAGE>` tag and one image asset are exported.
- Boolean operations are supported and will also result in the export of a single `<IMAGE>` tag and one image asset.