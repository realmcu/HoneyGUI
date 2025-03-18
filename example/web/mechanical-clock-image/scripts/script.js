window.onload = function() {
    const hourHand = document.querySelector('.hour-hand img');
    const minuteHand = document.querySelector('.minute-hand img');
    const secondHand = document.querySelector('.second-hand img');

    function updateClock() {
        const now = new Date();
        const seconds = now.getSeconds();
        const minutes = now.getMinutes();
        const hours = now.getHours() % 12; 
        const milliseconds = now.getMilliseconds();
        const secondFraction = seconds + milliseconds / 1000;
        const secondsDegrees = ((secondFraction / 60) * 360) + 90;
        const minutesDegrees = ((minutes / 60) * 360) + ((secondFraction / 60) * 6) + 90;
        const hoursDegrees = ((hours / 12) * 360) + ((minutes / 60) * 30) + 90;

        secondHand.style.transform = `rotate(${secondsDegrees}deg)`;
        minuteHand.style.transform = `rotate(${minutesDegrees}deg)`;
        hourHand.style.transform = `rotate(${hoursDegrees}deg)`;
    }

    setInterval(updateClock, 10);
    updateClock(); 
};
