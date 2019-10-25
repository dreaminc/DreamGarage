pipeline {
  agent any
  
  checkout([$class:'GitSCM', 
    branches: [[name: "refs/heads/" + env.BRANCH_NAME]],
        doGenerateSubmoduleConfigurations: false,
        extensions:[[$class:'GitLFSPull']]
    ])

  stages {

    stage('Build') {
      steps {
        //cmake arguments: '-DCMAKE_TOOLCHAIN_FILE=~/Projects/vcpkg/scripts/buildsystems/vcpkg.cmake', installation: 'InSearchPath'
        cmake installation: 'InSearchPath'
        cmakeBuild buildType: 'Release', cleanBuild: true, installation: 'InSearchPath', steps: [[withCmake: true]]
      }
    }

    stage('Test') {
      steps {
        echo 'Testing...'
      }
    }

    stage('Deploy') {
      steps {
        echo 'Deploying...'  
      }
    }
  }
}
