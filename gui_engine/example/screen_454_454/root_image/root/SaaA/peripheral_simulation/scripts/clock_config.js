function updateValue(id) {
    document.getElementById(id + 'Val').innerText = document.getElementById(id).value;
    sendData();
}

function sendData() {
    const data = {
        weather: [
            {
                low: parseInt(document.getElementById('tempLow').value),
                high: parseInt(document.getElementById('tempHigh').value),
                cur: parseInt(document.getElementById('tempCur').value),
                condition_1: document.getElementById('condition_1').value,
                condition_2: document.getElementById('condition_2').value,
                condition_3: document.getElementById('condition_3').value,
                condition_4: document.getElementById('condition_4').value,
                condition_5: document.getElementById('condition_5').value,
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
                stand: parseInt(document.getElementById('activityStand').value),
            }
        ],
        heart_rate: [
            {
                AM12: parseInt(document.getElementById('heart_rate12AM').value),
                AM6: parseInt(document.getElementById('heart_rate6AM').value),
                PM12: parseInt(document.getElementById('heart_rate12PM').value),
                PM6: parseInt(document.getElementById('heart_rate6PM').value)
            }
        ]
    };

    fetch('/simulate', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify(data),
    }).then(response => response.json())
      .then(data => {
          document.getElementById('output').innerText = data.output;
      });
}

window.onload = function() {
    // Send initial data to the server
    sendData();
};
