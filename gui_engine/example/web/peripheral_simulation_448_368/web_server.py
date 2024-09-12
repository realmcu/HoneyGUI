from flask import Flask, request, jsonify, render_template_string
import json
import subprocess
import http.server
import socketserver
import webbrowser
import threading
import os

app = Flask(__name__)

HTML_TEMPLATE = '''
<html>
<head>
    <title>Simulator Interface</title>
    <style>
        input[type=range] {
            width: 300px;
        }
    </style>
</head>
<body>
    <h1>Simulator Interface</h1>
    <div>
        <h3>Temperature:</h3>
        Low: <input type="range" id="tempLow" min="18" max="26" value="0" oninput="updateValue('tempLow')"><span id="tempLowVal"></span><br>
        High: <input type="range" id="tempHigh" min="30" max="37" value="0" oninput="updateValue('tempHigh')"><span id="tempHighVal"></span><br>
        Current: <input type="range" id="tempCur" min="18" max="37" value="0" oninput="updateValue('tempCur')"><span id="tempCurVal"></span><br>
    </div>
    <div>
        <h3>Compass:</h3>
        Degree: <input type="range" id="compDegree" min="0" max="359" value="0" oninput="updateValue('compDegree')"><span id="compDegreeVal"></span><br>
    </div>
    <div>
        <h3>Activity:</h3>
        Move: <input type="range" id="activityMove" min="1" max="5000" value="0" oninput="updateValue('activityMove')"><span id="activityMoveVal"></span><br>
        Exercise: <input type="range" id="activityEx" min="1" max="30" value="0" oninput="updateValue('activityEx')"><span id="activityExVal"></span><br>
        Stand: <input type="range" id="activityStand" min="1" max="12" value="0" oninput="updateValue('activityStand')"><span id="activityStandVal"></span><br>
    </div>
    <pre id="output"></pre>
    <script>
        function updateValue(id) {
            document.getElementById(id + 'Val').innerText = document.getElementById(id).value;
            sendData();
        }

        function sendData() {
            const data = {
                temperature: [
                    {
                        low: parseInt(document.getElementById('tempLow').value),
                        high: parseInt(document.getElementById('tempHigh').value),
                        cur: parseInt(document.getElementById('tempCur').value)
                    }
                ],
                compass: [
                    {
                        degree: parseInt(document.getElementById('compDegree').value),
                    }
                ],
                activity: [
                    {
                        move: parseInt(document.getElementById('activityMove').value),
                        ex: parseInt(document.getElementById('activityEx').value),
                        stand: parseInt(document.getElementById('activityStand').value)
                    }
                ]
            };

            fetch('/simulate', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify(data),
            })
            .then(response => response.json())
            .then(data => {
                document.getElementById('output').innerText = data.output;
            });
        }
        window.onload = function() {
            // Initialize the display spans with the default slider values
            const initialValues = {
                tempLow: 18, // Change these values as needed
                tempHigh: 37,
                tempCur: 22,
                compDegree: 1,
                activityMove: 1,
                activityEx: 1,
                activityStand: 1
            };

            for (const id in initialValues) {
                const slider = document.getElementById(id);
                slider.value = initialValues[id];
                document.getElementById(id + 'Val').innerText = initialValues[id];
            }

            // Send initial data to the server
            sendData();
        };
    </script>
</body>
</html>
'''

PORT = 8000

@app.route('/')
def index():
    return render_template_string(HTML_TEMPLATE)

@app.route('/simulate', methods=['POST'])
def simulate():
    input_data = request.json
    # 将数据写入JSON文件
    json_filename = 'simulation_data.json'
    with open(json_filename, 'w') as json_file:
        json.dump(input_data, json_file, indent=4)

    # 调用C语言模拟器读取JSON文件
    # result = subprocess.run(['./simulator', json_filename], capture_output=True, text=True)
    # return jsonify({'output': result.stdout})

def open_browser():
    url = f"http://localhost:{PORT}"
    print(f"Opening {url}")
    webbrowser.open(url)

if __name__ == '__main__':
    open_browser()
    app.run(port=PORT, debug=False)
    
