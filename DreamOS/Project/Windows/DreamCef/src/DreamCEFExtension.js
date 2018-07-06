var Dream;

if (!Dream) {
    Dream = {};
    Dream.Forms = {};
    Dream.Browser = {};
    Dream.Browser.INPUT_SELECTOR = 'input[type=password], input[type=search], input[type=email], input[type=url], input[type=tel], input[type=number], textarea';
    Dream.Browser.FOCUSED_INPUT_SELECTOR = 'input[type=password]:focus, input[type=search]:focus, input[type=email]:focus, input[type=url]:focus, input[type=tel]:focus, input[type=number]:focus, textarea:focus';

    (function() {
        Dream.Browser.getFocusedInput = function () {
            return document.querySelector(Dream.Browser.FOCUSED_INPUT_SELECTOR);
        }
    })();

    (function () {
        Dream.Browser.getFormInputs = function (input) {
            var allInputs = input.form.querySelectorAll(Dream.Browser.INPUT_SELECTOR);

            var visibleInputs = Array();
            for (var i = 0; i < allInputs.length; i++) {
                if (window.getComputedStyle(allInputs[i]).display !== 'none') visibleInputs.push(allInputs[i]);
            }

            return visibleInputs;        }
    })();
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

        var focusedInput = Dream.Browser.getFocusedInput();

        if (focusedInput == null || focusedInput.form === undefined) return canTabNext(false);

        var allInputs = Dream.Browser.getFormInputs(focusedInput);

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

        var focusedInput = Dream.Browser.getFocusedInput();

        if (focusedInput == null || focusedInput.form === undefined) return canTabPrevious(false);

        var allInputs = Dream.Browser.getFormInputs(focusedInput);

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
        var focusedInput = Dream.Browser.getFocusedInput();

        if (focusedInput == null || focusedInput.form === undefined) return false;

        var allInputs = Dream.Browser.getFormInputs(focusedInput);

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
        var focusedInput = Dream.Browser.getFocusedInput();

        if (focusedInput == null || focusedInput.form === undefined) return false;

        var allInputs = Dream.Browser.getFormInputs(focusedInput);

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

(function () {
    Dream.Browser.isInputFocused = function () {
        native function isInputFocused(inputFocused);

        var focusedInput = Dream.Browser.getFocusedInput();

        if (focusedInput == null) return isInputFocused(false);

        return isInputFocused(true);
    }
})();
