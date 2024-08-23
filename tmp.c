Hello,

To debug the RGD and determine the root cause of this error, it's necessary to rebuild the LRT module, specifically version 6.0, which BDR references, in order to generate the *.pdb files that contain debugging information.

Unfortunately, this version is no longer available on GitHub, so we don’t have access to the source code; we only have the generated package.

We attempted to rebuild BDR using a newer version of LRT. Although the build was successful, when we ran the batch, it encountered issues loading some dynamic libraries (likely because LRT and PFC are not aligned on the same dependency versions).

[CAPTURE]

As a result, I reviewed the log and code. I found that this error occurs when it attempts to select data from the "country" table and finds no results.

Below is the selection query:

[QUERY]

In my opinion, the issue may be due to either the CODE_COUNTY_ID not existing, or the CHM_VALUE in the 'ENREGISTREMENT' being empty.

Additionally, please note that the majority of the team members, including those with a deeper understanding of the functional aspects, will be back on September 2nd. I'll follow up with them when they return to get their insights on this issue.
