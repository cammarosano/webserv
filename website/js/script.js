let plusButton = document.getElementById("plus");
let minusButton = document.getElementById("minus");
let countp = document.getElementById("count");

let count = 0;

plusButton.addEventListener('click', () => {
    count++;
    countp.innerHTML = count;
});


minusButton.addEventListener('click', () => {
    count--;
    countp.innerHTML = count;
});