# Pixso XML Export Plugin

This is a plugin designed for Pixso, aimed at simplifying the workflow for designers and developers by converting design drafts into a specific XML format and exporting all related image assets.

The plugin is built on Pixso's official Vue template. It parses properties such as layer position, size, text styles, and image fills to automatically generate well-structured XML code suitable for custom UI frameworks.

## Key Features

1.  **XML Generation**: Creates a structured XML string from any selected layer (typically a top-level frame).
2.  **Asset Discovery**: Automatically scans for all layers with image fills within the selected layer hierarchy and adds them to the export queue.
3.  **Image Export**: Exports all discovered images as a single `.zip` file with one click.
4.  **Component Parsing**: Correctly handles component instances and parses their child layers.
5.  **Text Processing**: Extracts text content, font, font size, and color properties.
6.  **Special Keyword Handling**: Applies custom logic to nested frames whose names contain special keywords (like `list`), generating unique XML tags (e.g., `<LIST>`) and employing a specialized asset collection strategy.

## Development Environment Setup

### Prerequisites

1.  **Node.js**: Required to the project build scripts. We recommend using the latest LTS version of [Node.js](https://nodejs.org/en).

2.  **Pixso Desktop Client**: The plugin can only be loaded in the Pixso desktop application, although web-based testing is supported.

### Installation and Build

1.  **Install Dependencies**
    After navigating to the project directory, run the following command to install all necessary dependencies listed in the `package.json` file.
    ```bash
    npm install
    ```
    This will create a `node_modules` folder in your project, containing all the required packages.

2.  **Build the Plugin**
    This command will generate a `main.js` file for development, which contains the compiled plugin files:
    ```bash
    npm run build
    ```
    You need to rebuild every time you modify the relevant files (`main.ts`, `app.vue`) to regenerate the `main.js` file.
    Alternatively, use the command below to start Pixso's hot-reloading watcher, which will automatically build after you save your changes:
    ```bash
    npm run dev
    ```

### Loading the Plugin in Pixso

1.  Open the Pixso desktop client.
2.  Navigate via the top-right main menu to: `Plugins` > `Development` > `Import plugin...`.
3.  In the file dialog that opens, navigate to your project directory and select the `manifest.json` file.
4.  Once imported successfully, the plugin will appear under the `Plugins` > `Development` menu, ready to be run and tested.

## User Guide

Follow these steps to export your design using the plugin.

### Step 1: Open the Plugin

In the Pixso application, open the plugin by navigating to `Plugins` > `Development` > `D2XML` > `Run`.

### Step 2: Select the Main Layer

On your canvas, select the top-level frame(s) you wish to export. The plugin requires a selection to work. This frame typically represents a full screen or a complete UI view.

### Step 3: Generate XML

-   Click the **"Generate XML"** button in the plugin panel.
-   The plugin will process your selection and display the generated XML code in the text area.
-   A notification will pop up, informing you how many images were found in the selected layers.

### Step 4: Export Images and XML File

-   After the XML is generated, if any images are detected, the "Export" button will become enabled.
-   Click this button, and it will package all discovered images into a `.zip` file, which will be downloaded by your browser.
-   **Note**: You must run "Generate XML" first to discover the images before you can export them.

### Step 5: Split the XML File for HoneyGUI

-   Copy the `design.xml` file from the downloaded zip package to the `tool/D2XML/split_xml/` directory.
-   Double-click the `split_xml.bat` script in the same directory to get multiple XML files. Copy these files into the HoneyGUI project folder, which is typically `root/root_image/`.

## Core Concept: Special Keyword Handling

To support dynamic layouts like lists and grids, the plugin uses a rule system based on layer naming, which is activated when a layer name contains specific keywords.

### Trigger Conditions

To create controls not supported by MasterGo, this special logic is triggered when the name of a nested frame (i.e., a frame inside another frame) **contains** any of the following keywords (case-insensitive):
-   `cellular`
-   `list`
-   `video`

**Important Note**: This rule **does not apply** to the top-level frames you initially select on the canvas.

### Processing Effects

If a nested frame named `my_contact_list` is processed, the following occurs:

1.  **XML Tag is Changed**: In the output XML, the frame will be converted to a tag matching the keyword, instead of the generic `<WIN>`.
2.  **Child Nodes are Not Rendered as XML**: None of the layers inside `my_contact_list` will be converted into XML elements.
3.  **Specialized Image Collection**: The plugin will still search inside the `my_contact_list` frame for images. However, if it encounters a descendant layer whose name **exactly matches** a keyword (e.g., a layer named precisely `list`), then that layer and its entire subtree will be skipped.
