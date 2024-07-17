const canvas = document.getElementById('clockCanvas');
const ctx = canvas.getContext('2d');
const radius = canvas.width / 2;
ctx.translate(radius, radius);

function drawClock() {
    drawFace(ctx, radius);
    drawTime(ctx, radius);
    drawNumbers(ctx, radius);
}

function drawFace(ctx, radius) {

    ctx.beginPath();
    ctx.arc(0, 0, radius-3, 0, 2 * Math.PI);
    ctx.fillStyle = '#fff';
    ctx.fill();

    ctx.strokeStyle = '#333';
    ctx.lineWidth = radius * 0.01;
    ctx.stroke();
}

function drawNumbers(ctx, radius) {
    ctx.font = radius * 0.15 + "px Arial";
    ctx.fillStyle = "#333";  
    ctx.textBaseline = "middle";
    ctx.textAlign = "center";
    for (let num = 1; num <= 12; num++) {
        const ang = num * Math.PI / 6;
        ctx.rotate(ang);
        ctx.translate(0, -radius * 0.85); 
        ctx.rotate(-ang);
        ctx.fillText(num.toString(), 0, 0);
        ctx.rotate(ang);
        ctx.translate(0, radius * 0.85); 
        ctx.rotate(-ang);
    }
}
function drawTime(ctx, radius) {
    const now = new Date();
    const hours = now.getHours();
    const minutes = now.getMinutes();
    const seconds = now.getSeconds();
    const milliseconds = now.getMilliseconds();
    const secondFraction = seconds + milliseconds / 1000;

    // Hour
    const hourHandLength = radius * 0.5;
    const hourAngle = ((hours % 12) * Math.PI / 6) + (minutes * Math.PI / (6 * 60)) + (seconds * Math.PI / (360 * 60));
    drawHand(ctx, hourAngle, hourHandLength, radius * 0.05);

    // Minute
    const minuteHandLength = radius * 0.8;
    const minuteAngle = (minutes * Math.PI / 30) + (seconds * Math.PI / (30 * 60));
    drawHand(ctx, minuteAngle, minuteHandLength, radius * 0.03);

    // Second
    const secondHandLength = radius * 0.9;
    const secondAngle = secondFraction * Math.PI / 30;
    drawHand(ctx, secondAngle, secondHandLength, radius * 0.01);
    
}

function drawHand(ctx, pos, length, width) {
    ctx.beginPath();
    ctx.lineWidth = width;
    ctx.lineCap = "round";
    ctx.moveTo(0, 0);
    ctx.rotate(pos);
    ctx.lineTo(0, -length);
    ctx.strokeStyle = width == radius * 0.01 ? "red" : "black"; 
    ctx.stroke();
    ctx.rotate(-pos);
}

function updateClock() {
    ctx.clearRect(-radius, -radius, canvas.width, canvas.height);
    drawClock();
    requestAnimationFrame(updateClock);
}

updateClock();
