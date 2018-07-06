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

        var focusedInput = document.querySelector('input[type=text]:focus, input[type=password]:focus, textarea:focus');

        if (focusedInput == null || focusedInput.form === undefined) return canTabNext(false);

        var allInputs = focusedInput.form.querySelectorAll('input[type=text], input[type=password], textarea');

        var foundFocused = false;
        for (var i = 0; i < allInputs.length; i++) {
            if (foundFocused === true) return canTabNext(true);
            if (allInputs[i] === focusedInput) foundFocused = true;
        }

        return canTabNext(false);
    }
})();

(function () {
    Dream.Browser.canTabPrevious = function () {

        native function canTabPrevious(canPrevious);

        var focusedInput = document.querySelector('input[type=text]:focus, input[type=password]:focus, textarea:focus');

        if (focusedInput == null || focusedInput.form === undefined) return canTabPrevious(false);

        var allInputs = focusedInput.form.querySelectorAll('input[type=text], input[type=password], textarea');

        var foundFocused = false;
        for (var i = allInputs.length - 1; i >= 0; i--) {
            if (foundFocused === true) return canTabPrevious(true);
            if (allInputs[i] === focusedInput) foundFocused = true;
        }

        return canTabPrevious(false);

    }
})();
(function () {
    Dream.Browser.tabNext = function () {
        var focusedInput = document.querySelector('input[type=text]:focus, input[type=password]:focus, textarea:focus');

        if (focusedInput == null || focusedInput.form === undefined) return false;

        var allInputs = focusedInput.form.querySelectorAll('input[type=text], input[type=password], textarea');

        var foundFocused = false;
        for (var i = 0; i < allInputs.length; i++) {
            if (foundFocused === true) {
                allInputs[i].focus();
                break;
            }

            if (allInputs[i] === focusedInput) foundFocused = true;
        }
    }
})();

(function () {
    Dream.Browser.tabPrevious = function () {
        var focusedInput = document.querySelector('input[type=text]:focus, input[type=password]:focus, textarea:focus');

        if (focusedInput == null || focusedInput.form === undefined) return false;

        var allInputs = focusedInput.form.querySelectorAll('input[type=text], input[type=password], textarea');

        var foundFocused = false;
        for (var i = allInputs.length - 1; i >= 0; i--) {
            if (foundFocused === true) {
                allInputs[i].focus();
                break;
            }

            if (allInputs[i] === focusedInput) foundFocused = true;
        }
    }
})();