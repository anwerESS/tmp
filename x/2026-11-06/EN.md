“I’m still blocked on development tasks, so I’ve moved forward with the Data Transformer alerts.

Could I share my screen quickly?

[Logs PROD]  
Here, I’ve found a filter to retrieve the Data Transformer logs in production. I’m filtering on stderr, and the next step is to check which messages correspond to actual processing failures.

[Connecteur DEV]  
In dev, I’ve looked at how to set up an email connector. I’ll need the SMTP settings, including the host, port and sender address. Then we can test the email delivery and simulate a failure in dev to check the alert.

[Connecteurs PROD]  
In production, there are already a few connectors, including this email connector called ‘CAGIP - Send Mail’. We need to check whether we can reuse it or whether we need a dedicated one.

Donatien, do you know who could help us confirm that and provide the SMTP settings for dev?”

That's it from my side.
