var Dream;

if (!Dream) {
    Dream = {};
    Dream.Form = {};
}

(function() {
  Dream.Form.cancel = function() {
      native function cancel();
      return cancel();
  }
})();

(function() {
  Dream.Form.success = function() {
      native function success();
      return success();
  }
})();

(function() {
  Dream.Form.setCredentials = function(refreshToken, accessToken) {
      native function setCredentials(refreshToken, accessToken);
      return setCredentials(refreshToken, accessToken);
  }
})();

(function() {
  Dream.Form.setEnvironmentId = function(environmentId) {
      native function setEnvironmentId(environmentId);
      return setEnvironmentId(environmentId);
  }
})();