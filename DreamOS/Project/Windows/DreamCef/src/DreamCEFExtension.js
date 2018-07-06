var Dream;

if (!Dream) {
    Dream = {};
    Dream.Forms = {};
    Dream.Browser = {};
}

(function() {
  Dream.Forms.cancel = function() {
      native function cancel();
      return cancel();
  }
})();

(function() {
  Dream.Forms.success = function() {
      native function success();
      return success();
  }
})();

(function() {
  Dream.Forms.setCredentials = function(refreshToken, accessToken) {
      native function setCredentials(refreshToken, accessToken);
      return setCredentials(refreshToken, accessToken);
  }
})();

(function() {
  Dream.Forms.setEnvironmentId = function(environmentId) {
      native function setEnvironmentId(environmentId);
      return setEnvironmentId(environmentId);
  }
})();

(function () {
    Dream.Browser.canTabNext = function () {

        native function canTabNext(canNext);
        var thisElement = document.querySelector(':focus');

        var canNext = false;
        //var thisElement = document.activeElement;

        while (thisElement = thisElement.nextElementSibling) {
            if (thisElement == null) {
                break;
            }
            if (thisElement.tagName.toLowerCase() === "input" && thisElement.type.toLowerCase() === "text") {
                canNext = true;
                break;
            }
            if (thisElement.tagName.toLowerCase() === "input" && thisElement.type.toLowerCase() === "password") {
                canNext = true;
                break;
            }
            if (thisElement.tagName.toLowerCase() === "textarea") {
                canNext = true;
                break;
            }
        }

//        return canTabNext(canNext);
        return canNext;
    }
})();

(function () {
    Dream.Browser.canTabPrevious = function () {

        native function canTabPrevious(canPrevious);

        var canPrevious = false;
        var thisElement = document.activeElement;

        while (thisElement = thisElement.previousElementSibling) {
            if (thisElement == null) {
                break;
            }
            if (thisElement.tagName.toLowerCase() === "input" && thisElement.type.toLowerCase() === "text") {
                canPrevious = true;
                break;
            }
            if (thisElement.tagName.toLowerCase() === "input" && thisElement.type.toLowerCase() === "password") {
                canPrevious = true;
                break;
            }
            if (thisElement.tagName.toLowerCase() === "textarea") {
                canPrevious = true;
                break;
            }
        }

        return canTabPrevious(canPrevious);
    }
})();
(function () {
    Dream.Browser.tabNext = function () {
        var thisElement = document.activeElement;


        while (thisElement = thisElement.nextElementSibling) {
            if (thisElement == null) {
                break;
            }
            if (thisElement.tagName.toLowerCase() === "input" && thisElement.type.toLowerCase() === "text") {
                thisElement.focus();
                break;
            }
            if (thisElement.tagName.toLowerCase() === "input" && thisElement.type.toLowerCase() === "password") {
                thisElement.focus();
                break;
            }
            if (thisElement.tagName.toLowerCase() === "textarea") {
                thisElement.focus();
                break;
            }
        }
    }
})();

(function () {
    Dream.Browser.tabPrevious = function () {
        var thisElement = document.activeElement;

        while (thisElement = thisElement.previousElementSibling) {
            if (thisElement == null) {
                break;
            }
            if (thisElement.tagName.toLowerCase() === "input" && thisElement.type.toLowerCase() === "text") {
                thisElement.focus();
                break;
            }
            if (thisElement.tagName.toLowerCase() === "input" && thisElement.type.toLowerCase() === "password") {
                thisElement.focus();
                break;
            }
            if (thisElement.tagName.toLowerCase() === "textarea") {
                thisElement.focus();
                break;
            }
        }
    }
})();