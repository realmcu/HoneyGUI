document.addEventListener("DOMContentLoaded", function() {
    const lightsContainer = document.getElementById('lights-container');

    fetch('light_config.json')
        .then(response => response.json())
        .then(config => {
            const lightsConfig = config.lights;
            lightsConfig.forEach(lightConfig => {
                const lightDiv = document.createElement('div');
                lightDiv.classList.add('light');
                lightDiv.id = `light-${lightConfig.id}`;
                lightDiv.style.backgroundImage = lightConfig.state === 'on' ? 
                    "url('resources/light_on.png')" : "url('resources/light_off.png')";

                const indexText = document.createElement('span');
                indexText.textContent = lightConfig.id;
                lightDiv.appendChild(indexText);

                lightDiv.addEventListener('click', () => {
                    if (lightDiv.classList.contains('on')) {
                        lightDiv.classList.remove('on');
                        lightDiv.style.backgroundImage = "url('resources/light_off.png')";
                    } else {
                        lightDiv.classList.add('on');
                        lightDiv.style.backgroundImage = "url('resources/light_on.png')";
                    }
                });

                lightsContainer.appendChild(lightDiv);
            });
        })
        .catch(error => console.error('Error loading configuration file:', error));
});
