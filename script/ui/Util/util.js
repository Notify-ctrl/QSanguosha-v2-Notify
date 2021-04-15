.pragma library

function convertNumber(number) {
    if (number === 1)
        return "A";
    if (number >= 2 && number <= 10)
        return number;
    if (number >= 11 && number <= 13) {
        var strs = ["J", "Q", "K"];
        return strs[number - 11];
    }
    return "";
}

Array.prototype.contains = function(element) {
    for (var i = 0; i < this.length; i++) {
        if (this[i] === element)
            return true;
    }
    return false;
}

Array.prototype.prepend = function() {
    this.splice(0, 0, ...arguments);
}

var kingdomColor = {
    wei : "#547998",
    shu : "#D0796C",
     wu : "#4DB873",
    qun : "#8A807A",
    god : "#96943D",
unknown : "#96943D"
};
