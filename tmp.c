
Subject: Debugging RDG with New LRT v6.0.CD Version

Hi,

Following Chokri's email regarding the RDG debugging, I wanted to propose a solution since we couldn’t retrieve the old LRT v6.0.CD version.

We've decided to take a different approach. The solution I suggest involves debugging the BDR with a version very close to 6.0.CD, after making a few configurations in Visual Studio.

Since the RDG function code hasn't changed in a long time, its behavior should remain consistent. On our side, we created a new version of 6.0.CD after resolving issues with the other dependencies.

Before scheduling a meeting, could you please prepare your environment using the following steps:

Clone the newly created LRT 6.0.CD version from this repo: <URL>
Run the command mvn clean initialize
Once you're set up, I will guide you through debugging and stepping into the RDG752 function in BDR.

Best regards,
[Your Name
