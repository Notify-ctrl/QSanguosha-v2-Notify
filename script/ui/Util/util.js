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
