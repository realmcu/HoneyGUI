function updateValue(id) {
    document.getElementById(id + 'Val').innerText = document.getElementById(id).value;
    sendData();
}

function sendData() {
    const data = {
        weather: [
            {
                range_min: parseInt(document.getElementById('tempMin').value),
                range_max: parseInt(document.getElementById('tempMax').value),
                low: parseInt(document.getElementById('tempLow').value),
                high: parseInt(document.getElementById('tempHigh').value),
                current: parseInt(document.getElementById('tempCur').value),
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
                move_max: parseInt(document.getElementById('moveMax').value),
                exercise_max: parseInt(document.getElementById('exMax').value),
                stand_max: parseInt(document.getElementById('standMax').value),
                move: parseInt(document.getElementById('activityMove').value),
                exercise: parseInt(document.getElementById('activityEx').value),
                stand: parseInt(document.getElementById('activityStand').value),
            }
        ],
        heart_rate: [
            {
                range_min: parseInt(document.getElementById('hrMin').value),
                range_max: parseInt(document.getElementById('hrMax').value),
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

function updateTempRange() {
    var minValue = document.getElementById('tempMin').value;
    var maxValue = document.getElementById('tempMax').value;
    document.getElementById('tempLow').setAttribute('min', minValue);
    document.getElementById('tempLow').setAttribute('max', maxValue);
    document.getElementById('tempHigh').setAttribute('min', minValue);
    document.getElementById('tempHigh').setAttribute('max', maxValue);
    document.getElementById('tempCur').setAttribute('min', parseInt(document.getElementById('tempLow').value));
    document.getElementById('tempCur').setAttribute('max', parseInt(document.getElementById('tempHigh').value));

    // Ensure the current values are within the new range
    if (parseInt(document.getElementById('tempLow').value) < minValue) {
        document.getElementById('tempLow').value = minValue;
    }
    if (parseInt(document.getElementById('tempLow').value) > maxValue) {
        document.getElementById('tempLow').value = maxValue;
    }
    if (parseInt(document.getElementById('tempCur').value) < parseInt(document.getElementById('tempLow').value)) {
        document.getElementById('tempCur').value = parseInt(document.getElementById('tempLow').value);
    }
    if (parseInt(document.getElementById('tempCur').value) > parseInt(document.getElementById('tempHigh').value)) {
        document.getElementById('tempCur').value = parseInt(document.getElementById('tempHigh').value);
    }
    if (parseInt(document.getElementById('tempHigh').value) < minValue) {
        document.getElementById('tempHigh').value = minValue;
    }
    if (parseInt(document.getElementById('tempHigh').value) > maxValue) {
        document.getElementById('tempHigh').value = maxValue;
    }
    // Update displayed values
    updateValue('tempLow');
    updateValue('tempCur');
    updateValue('tempHigh');
}

function updateActivityMax() {
    var movemaxValue = document.getElementById('moveMax').value;
    document.getElementById('activityMove').setAttribute('max', movemaxValue);
    var exmaxValue = document.getElementById('exMax').value;
    document.getElementById('activityEx').setAttribute('max', exmaxValue);
    var standmaxValue = document.getElementById('standMax').value;
    document.getElementById('activityStand').setAttribute('max', standmaxValue);
    // Ensure the current values are within the new range
    if (parseInt(document.getElementById('activityMove').value) > movemaxValue) {
        document.getElementById('activityMove').value = movemaxValue;
    }
    if (parseInt(document.getElementById('activityEx').value) > exmaxValue) {
        document.getElementById('activityEx').value = exmaxValue;
    }
    if (parseInt(document.getElementById('activityStand').value) > standmaxValue) {
        document.getElementById('activityStand').value = standmaxValue;
    }

    updateValue('activityMove');
    updateValue('activityEx');
    updateValue('activityStand');
}

function updateHrRange() {
    var minValue = document.getElementById('hrMin').value;
    var maxValue = document.getElementById('hrMax').value;
    document.getElementById('heart_rate12AM').setAttribute('min', minValue);
    document.getElementById('heart_rate12AM').setAttribute('max', maxValue); //heart_rate6AM
    document.getElementById('heart_rate6AM').setAttribute('min', minValue);
    document.getElementById('heart_rate6AM').setAttribute('max', maxValue);
    document.getElementById('heart_rate12PM').setAttribute('min', minValue);
    document.getElementById('heart_rate12PM').setAttribute('max', maxValue);
    document.getElementById('heart_rate6PM').setAttribute('min', minValue);
    document.getElementById('heart_rate6PM').setAttribute('max', maxValue);

    // Ensure the current values are within the new range
    if (parseInt(document.getElementById('heart_rate12AM').value) < minValue) {
        document.getElementById('heart_rate12AM').value = minValue;
    }
    if (parseInt(document.getElementById('heart_rate12AM').value) > maxValue) {
        document.getElementById('heart_rate12AM').value = maxValue;
    }
    if (parseInt(document.getElementById('heart_rate6AM').value) < minValue) {
        document.getElementById('heart_rate6AM').value = minValue;
    }
    if (parseInt(document.getElementById('heart_rate6AM').value) > maxValue) {
        document.getElementById('heart_rate6AM').value = maxValue;
    }
    if (parseInt(document.getElementById('heart_rate12PM').value) < minValue) {
        document.getElementById('heart_rate12PM').value = minValue;
    }
    if (parseInt(document.getElementById('heart_rate12PM').value) > maxValue) {
        document.getElementById('heart_rate12PM').value = maxValue;
    }
    if (parseInt(document.getElementById('heart_rate6PM').value) < minValue) {
        document.getElementById('heart_rate6PM').value = minValue;
    }
    if (parseInt(document.getElementById('heart_rate6PM').value) > maxValue) {
        document.getElementById('heart_rate6PM').value = maxValue;
    }

    updateValue('heart_rate12AM');
    updateValue('heart_rate6AM');
    updateValue('heart_rate12PM');
    updateValue('heart_rate6PM');
}

window.onload = function() {
    // Send initial data to the server
    sendData();
};
