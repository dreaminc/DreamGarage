var Dream;

if (!Dream) {
    Dream = {};
    Dream.Forms = {};
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