document.addEventListener("DOMContentLoaded", function () {
    const lightsContainer = document.getElementById('lights-container');

    function colorTemperatureToRGB(temp) {
        temp = temp / 100;
        let red, green, blue;

        if (temp <= 66) {
            red = 255;
            green = temp;
            green = 99.4708025861 * Math.log(green) - 161.1195681661;

            if (temp <= 19) {
                blue = 0;
            } else {
                blue = temp - 10;
                blue = 138.5177312231 * Math.log(blue) - 305.0447927307;
            }
        } else {
            red = temp - 60;
            red = 329.698727446 * Math.pow(red, -0.1332047592);

            green = temp - 60;
            green = 288.1221695283 * Math.pow(green, -0.0755148492);

            blue = 255;
        }

        return [
            Math.max(0, Math.min(255, Math.round(red))),
            Math.max(0, Math.min(255, Math.round(green))),
            Math.max(0, Math.min(255, Math.round(blue)))
        ];
    }

    function updateLight(lightDiv, config) {
        const { state, brightness, colorTemperature } = config;
        const [r, g, b] = colorTemperatureToRGB(colorTemperature);

        if (state === 'on') {
            lightDiv.style.backgroundImage = "url('resources/light_on.png')";
            lightDiv.style.opacity = `${brightness / 100}`;
            lightDiv.style.backgroundColor = `rgba(${r}, ${g}, ${b}, 0.8)`;          
        } else {
            lightDiv.style.backgroundImage = "url('resources/light_off.png')";
            lightDiv.style.opacity = '1';
            lightDiv.style.backgroundColor = 'transparent'; 
        }
    }


    function renderLights(lightsConfig) {
        lightsContainer.innerHTML = "";

        lightsConfig.forEach(lightConfig => {
            const lightDiv = document.createElement('div');
            lightDiv.classList.add('light');
            lightDiv.id = `light-${lightConfig.id}`;
            const indexText = document.createElement('span');
            indexText.textContent = lightConfig.id;
            lightDiv.appendChild(indexText);

            updateLight(lightDiv, lightConfig);

            lightsContainer.appendChild(lightDiv);
        });
    }

    function loadConfiguration() {
        fetch('light_config.json')
            .then(response => response.json())
            .then(config => {
                const lightsConfig = config.lights;
                renderLights(lightsConfig);
            })
            .catch(error => console.error('Error loading configuration file:', error));
    }

    loadConfiguration();
    setInterval(loadConfiguration, 1000);
});