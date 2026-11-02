1
00:00:00,000 --> 00:00:01,000
Hello guys.

2
00:00:01,000 --> 00:00:18,520
Hi, hello, hi, hello, yeah, sorry, sorry, my headset was having some problem, yeah.

3
00:00:18,520 --> 00:00:20,960
Christine is not able to join today.

4
00:00:20,960 --> 00:00:21,960
Okay.

5
00:00:21,960 --> 00:00:22,960
Trust me.

6
00:00:22,960 --> 00:00:23,960
Sure.

7
00:00:24,960 --> 00:00:26,640
Let's proceed.

8
00:00:26,640 --> 00:00:33,800
So Manel is on leave for the next two weeks.

9
00:00:33,800 --> 00:00:44,000
So for maybe to start, we've been able to do some exchange on Friday where we actually

10
00:00:44,000 --> 00:00:51,480
had some changes in the code related to the injection.

11
00:00:51,480 --> 00:01:00,160
So we think like it might be, it might improve some of the things that because we might missed.

12
00:01:00,160 --> 00:01:08,120
So I'm here to ask the DBA to run a script in order to compare what we had before and

13
00:01:08,120 --> 00:01:15,400
what we have now in the goal table to see if there's anything that has been added.

14
00:01:15,600 --> 00:01:22,840
So, question for Joseph, I don't know if you have a way for to check maybe on the injection

15
00:01:22,840 --> 00:01:30,520
on the diamond table what you had maybe yesterday, I mean like last week compared to what you

16
00:01:30,520 --> 00:01:35,040
had today to see if there's any differences in the data.

17
00:01:35,040 --> 00:01:40,560
Is there any counts like a select count or something on the table?

18
00:01:40,560 --> 00:01:45,160
We didn't store the count or anything, but I can check if there's any newly ingested

19
00:01:45,160 --> 00:01:49,200
one based on the timestamp of the injection.

20
00:01:49,200 --> 00:01:50,200
Okay.

21
00:01:50,200 --> 00:01:51,200
Okay.

22
00:01:51,200 --> 00:01:55,680
I think that would be great just to see if you have new data that have been stored into

23
00:01:55,680 --> 00:01:59,280
the thing that might be.

24
00:01:59,280 --> 00:02:02,880
Do you have information on what was ingested into gold?

25
00:02:03,839 --> 00:02:12,320
Actually, she did a lot of things because she realized that even for example, the partial

26
00:02:12,320 --> 00:02:19,440
right and everything, there was nothing that was integrated in the goal table.

27
00:02:19,440 --> 00:02:28,800
And there was also, let's say in terms of data, I think the permissions also, there

28
00:02:28,880 --> 00:02:33,520
might be something that we might list in the past.

29
00:02:33,520 --> 00:02:40,960
So I'm still here to see further because on Friday we've done a select count on all the

30
00:02:40,960 --> 00:02:48,320
tables in order to compare with anyone that would be writing the DBA who is yet to execute

31
00:02:48,320 --> 00:02:56,080
and see not only the count, but also on the IDs, everything that has been maybe integrated

32
00:02:56,080 --> 00:02:58,200
as new data.

33
00:02:58,399 --> 00:03:04,640
So maybe after this is done, maybe I might share with you maybe what has been integrated

34
00:03:04,640 --> 00:03:07,200
or what is.

35
00:03:07,200 --> 00:03:08,200
Okay.

36
00:03:08,200 --> 00:03:09,200
Yeah.

37
00:03:09,200 --> 00:03:10,200
Okay, okay.

38
00:03:10,200 --> 00:03:11,200
Check, check.

39
00:03:11,200 --> 00:03:26,000
So on her side, she sent me an email, but I have no time to look at it yet because there

40
00:03:26,000 --> 00:03:32,480
were some tickets that she was working on, which she was, so this is the one that we

41
00:03:32,480 --> 00:03:35,800
tried to deploy on Friday.

42
00:03:35,800 --> 00:03:41,480
Also all these things, I think she needed to provide you with some information on these

43
00:03:41,480 --> 00:03:43,840
tickets, but it might be related.

44
00:03:43,840 --> 00:03:50,480
So I'll look to her email and I'll get back to you if needed.

45
00:03:51,079 --> 00:04:00,720
Anwar, for the dashboard, has she provided you with enough information for you to start

46
00:04:00,720 --> 00:04:06,920
looking into it from what we've stated here?

47
00:04:06,920 --> 00:04:10,120
So will you be able to pursue in her absence?

48
00:04:10,120 --> 00:04:11,120
Yes.

49
00:04:11,120 --> 00:04:13,320
Okay, okay.

50
00:04:13,320 --> 00:04:15,519
So I'll try and assign the ticket to you.

51
00:04:15,560 --> 00:04:16,960
Yes, I think.

52
00:04:22,160 --> 00:04:23,160
Okay.

53
00:04:25,760 --> 00:04:28,160
Okay, check, check, check.

54
00:04:28,160 --> 00:04:32,520
Apart from this, this has been also done.

55
00:04:32,520 --> 00:04:38,240
The mock-up, I think she has been able to, because she has some feedbacks and she needed

56
00:04:38,240 --> 00:04:42,600
to do something, I'll try and review them to see if anything, everything is okay or

57
00:04:42,600 --> 00:04:43,600
not.

58
00:04:44,480 --> 00:04:51,800
I think this one, she was trying to do it because HCN has provided some feedbacks on

59
00:04:51,800 --> 00:04:57,960
this one and she was looking into it, so I will look at him later to see if it's completed

60
00:04:57,960 --> 00:04:58,960
or not.

61
00:04:58,960 --> 00:05:03,520
Those are the main things that she worked on.

62
00:05:03,520 --> 00:05:06,320
So maybe Joseph, on your side.

63
00:05:06,320 --> 00:05:07,320
Yeah.

64
00:05:08,040 --> 00:05:19,480
So I worked on most of this ticket and it is mostly done.

65
00:05:19,480 --> 00:05:23,440
So for 1260.

66
00:05:26,000 --> 00:05:28,000
1260, yeah.

67
00:05:28,680 --> 00:05:30,680
He's...

68
00:05:34,680 --> 00:05:38,040
Yeah, I wrote my thing in there.

69
00:05:38,040 --> 00:05:42,680
So it has multiple things.

70
00:05:42,680 --> 00:05:47,120
So he...

71
00:05:47,120 --> 00:05:54,040
The LLM needs to understand the intent better, so I've improved on it.

72
00:05:54,080 --> 00:06:04,560
So the chat history, it stores a summary of the conversation so far and also stores the

73
00:06:04,560 --> 00:06:09,360
IDs that were retrieved previously so that it can be referenced to it.

74
00:06:09,360 --> 00:06:13,240
I put the example at the last paragraph there.

75
00:06:13,240 --> 00:06:14,520
I saw it.

76
00:06:14,520 --> 00:06:16,640
So it's not like...

77
00:06:16,640 --> 00:06:21,640
Do you remember my latest point or my first point, but it's more like when you use an

78
00:06:21,640 --> 00:06:27,560
ID and it gives you an example, like a response on an ID, he knows that this is the ID that

79
00:06:27,560 --> 00:06:30,200
you were referring to.

80
00:06:30,200 --> 00:06:31,200
Is that the idea?

81
00:06:31,200 --> 00:06:32,200
Yeah.

82
00:06:32,200 --> 00:06:38,719
It hinges on the IDs, like recommendation ID, assignment ID, like that.

83
00:06:38,719 --> 00:06:39,719
So it's...

84
00:06:39,719 --> 00:06:47,479
Because a lot of it requires to search through the IDs anyway, so it's the most efficient

85
00:06:48,320 --> 00:06:52,320
way to do it without overloading the context.

86
00:06:52,320 --> 00:06:53,320
Okay.

87
00:06:53,320 --> 00:06:57,600
So what have you improved then in this thing?

88
00:06:57,600 --> 00:07:07,640
Yeah, sometimes his follow-up question doesn't need to be referred to the previous ID, but

89
00:07:07,640 --> 00:07:09,640
the system mistook it.

90
00:07:09,800 --> 00:07:18,280
Maybe, for example, he asked in my first point, he first asked for assignments in 2023, and

91
00:07:18,280 --> 00:07:26,479
then he say, I want the same information, but for 2024 to 2025.

92
00:07:26,479 --> 00:07:35,560
So actually the LLM, I mean, the model should do a fresh search, but it went to use the

93
00:07:35,560 --> 00:07:42,080
IDs in 2023 to find them in 2024, but it got confused.

94
00:07:42,080 --> 00:07:48,000
So I made it clearer that it doesn't have to always go and use the previous IDs in this

95
00:07:48,000 --> 00:07:54,960
to be more aware of what the user intent is.

96
00:07:54,960 --> 00:07:55,960
Okay.

97
00:07:55,960 --> 00:07:57,840
All right.

98
00:07:57,840 --> 00:07:59,400
So this is the first one.

99
00:07:59,400 --> 00:08:04,480
I think there's something that he would need to retest when you deploy, but it will also

100
00:08:04,480 --> 00:08:11,560
be a part of maybe if there's a data mismatch on our side, he has to maybe...

101
00:08:11,560 --> 00:08:12,560
Yes, yes.

102
00:08:12,560 --> 00:08:18,520
There's two parts, because if the query is wrong, I have to fix it.

103
00:08:18,520 --> 00:08:23,360
But if he compares it with my audit, then it's for right now not really possible for

104
00:08:23,360 --> 00:08:29,440
me to check if the data we have is the same as my audit.

105
00:08:29,800 --> 00:08:30,800
Okay.

106
00:08:30,800 --> 00:08:40,960
We also have the validation from Etienne to ingest some data, I think, recently in pre-production

107
00:08:40,960 --> 00:08:43,760
and even in the dev.

108
00:08:43,760 --> 00:08:48,720
I think it's not really a dump and load, but it's just the injection data from my audit.

109
00:08:48,720 --> 00:08:57,840
So I think Manuel has been able to ingest all data in dev, and for pre-production he has

110
00:08:57,840 --> 00:08:58,840
no data.

111
00:08:58,840 --> 00:09:04,560
So if you were asking last time about having some permissions in some permission data in

112
00:09:04,560 --> 00:09:08,120
dev, I think now you should be able to see them.

113
00:09:08,120 --> 00:09:11,120
Okay, I'll check it.

114
00:09:11,120 --> 00:09:12,120
Okay.

115
00:09:12,120 --> 00:09:19,440
For the validation, the mismatch and everything, I will try and check this later on.

116
00:09:19,440 --> 00:09:20,440
Okay.

117
00:09:20,440 --> 00:09:23,120
It has been fixed.

118
00:09:23,120 --> 00:09:30,240
This is a bit similar to 1266, the very bottom one, by Luke.

119
00:09:30,240 --> 00:09:34,360
So it's also the increased conversation context?

120
00:09:34,360 --> 00:09:40,440
Do you also relate to the increased conversation context?

121
00:09:40,440 --> 00:09:42,000
Not a little bit.

122
00:09:42,000 --> 00:09:51,440
He asked a sequence of questions and the model should do a fresh query, but it referenced

123
00:09:51,520 --> 00:09:56,480
previously mentioned IDs when it shouldn't have.

124
00:09:56,480 --> 00:10:00,640
So I asked him to start a fresh search in the first one.

125
00:10:00,640 --> 00:10:02,640
I asked him to...

126
00:10:02,640 --> 00:10:05,640
No, in the first one I sent...

127
00:10:05,640 --> 00:10:06,640
No, this is his...

128
00:10:06,640 --> 00:10:07,640
Yeah, yeah, yeah, I see.

129
00:10:07,640 --> 00:10:08,640
I see.

130
00:10:08,640 --> 00:10:09,640
I saw it.

131
00:10:09,640 --> 00:10:10,640
Yeah.

132
00:10:10,640 --> 00:10:17,520
So I asked him to start a fresh check to ask that to see if his one came up.

133
00:10:17,600 --> 00:10:25,199
It seemed like the output is problematic.

134
00:10:25,199 --> 00:10:30,240
So now I need to investigate on what happened.

135
00:10:30,240 --> 00:10:36,600
Okay, all right.

136
00:10:36,600 --> 00:10:43,280
And the broken output format table, this one is...

137
00:10:43,360 --> 00:10:51,680
The information retrieved is okay, but it failed at...

138
00:10:51,680 --> 00:10:57,040
The table formatting was messed up, but the information is there actually.

139
00:10:57,040 --> 00:11:00,880
Something went wrong in the table formatting.

140
00:11:00,880 --> 00:11:08,560
So I just went to enhance the check for table formatting.

141
00:11:08,560 --> 00:11:12,720
So hopefully it's better.

142
00:11:13,280 --> 00:11:14,880
The information is in there actually.

143
00:11:16,480 --> 00:11:17,520
Okay, I see.

144
00:11:17,520 --> 00:11:17,920
I see.

145
00:11:17,920 --> 00:11:18,960
I can see over here.

146
00:11:19,600 --> 00:11:20,640
Yeah, it's in there.

147
00:11:20,640 --> 00:11:22,400
It just didn't get formatted properly.

148
00:11:22,400 --> 00:11:25,280
So I did some enhancement on the code level.

149
00:11:26,560 --> 00:11:28,400
So that's all for now.

150
00:11:28,400 --> 00:11:35,840
I will continue debugging on that one, the reply from Luke to investigate why the result

151
00:11:35,840 --> 00:11:36,960
got truncated.

152
00:11:37,040 --> 00:11:43,440
And then I will also begin working on the entity retrieval requested by Etienne.

153
00:11:44,720 --> 00:11:49,040
Okay, the one that I will hold, are you expecting something from...

154
00:11:51,040 --> 00:11:54,240
I think this is all the data mismatch.

155
00:11:55,040 --> 00:11:56,880
Okay, this is the data mismatch.

156
00:11:56,880 --> 00:11:58,000
So it's on...

157
00:11:58,000 --> 00:11:59,840
Okay, and this one is...

158
00:12:02,160 --> 00:12:03,920
All of them?

159
00:12:03,920 --> 00:12:04,960
Yeah, yeah, yeah.

160
00:12:04,960 --> 00:12:05,920
Okay, okay.

161
00:12:07,120 --> 00:12:07,760
All right.

162
00:12:07,760 --> 00:12:10,160
Oh, yeah, it's hard for me to focus on those now.

163
00:12:10,160 --> 00:12:11,200
I work on the other things.

164
00:12:12,320 --> 00:12:13,280
Sure, sure, sure.

165
00:12:13,280 --> 00:12:14,080
That's all for me.

166
00:12:15,920 --> 00:12:16,560
Okay, thanks.

167
00:12:19,600 --> 00:12:22,720
Do you want to maybe follow up on the INT1?

168
00:12:22,720 --> 00:12:27,840
Maybe another email just for them to see if they've been able to...

169
00:12:28,640 --> 00:12:29,360
Okay, thanks.

170
00:12:30,080 --> 00:12:30,580
Anwar?

171
00:12:32,320 --> 00:12:35,440
On my end on Friday, I had a session with Manel.

172
00:12:36,160 --> 00:12:41,440
She showed me how to deploy Squid AI and who could me throw a few tools.

173
00:12:43,040 --> 00:12:48,640
Regarding the SonarCube issue, the certificate issue,

174
00:12:49,280 --> 00:12:56,080
Vincent pointed me to the teams that might be managing the SonarCube server.

175
00:12:56,080 --> 00:13:01,120
So I sent them an email to check if the issue is related to the migration.

176
00:13:01,920 --> 00:13:04,880
And I am waiting for their response.

177
00:13:05,439 --> 00:13:11,760
Once I hear back, I will work with them to find a solution.

178
00:13:13,200 --> 00:13:18,720
Regarding the timeout issue, I'm still waiting for the required software and

179
00:13:18,720 --> 00:13:25,120
admin rights so I can run the application and start debugging.

180
00:13:26,800 --> 00:13:28,480
Yeah, actually for this one, maybe...

181
00:13:28,480 --> 00:13:31,920
I don't know if Manel has told you, but this is related to Redis.

182
00:13:32,640 --> 00:13:37,280
On Redis, actually, we have configured the Redis name.

183
00:13:37,280 --> 00:13:42,880
And the thing is, the pods actually, usually you can have some, I think,

184
00:13:43,520 --> 00:13:48,800
changes in the pods and everything related to the fact that, okay, the pod is down and

185
00:13:48,800 --> 00:13:51,920
has to be, let's say, switched to another pod and everything.

186
00:13:52,479 --> 00:13:56,959
So I think they were telling us that we have to switch to the IP address of the

187
00:13:58,800 --> 00:14:00,480
Redis instead of the name itself.

188
00:14:01,040 --> 00:14:10,560
And they were saying there's something called Sentinel that we can use in order for us to

189
00:14:10,560 --> 00:14:13,680
point directly to the IP address rather than the name.

190
00:14:13,680 --> 00:14:19,280
I think this would prevent us from having these intermittent timeout issues and everything.

191
00:14:19,840 --> 00:14:28,400
So maybe I'll try and see if I can give you a contact so that you can give you, let's say,

192
00:14:30,480 --> 00:14:36,800
the explanation about the Sentinel so that you can see how we can implement it.

193
00:14:38,400 --> 00:14:41,280
I'll see. I'll try and see if I can find you some name.

194
00:14:44,720 --> 00:14:47,200
Okay. So that's from my side.

195
00:14:48,640 --> 00:14:55,520
Okay. All right. So I think Christine is here.

196
00:14:55,840 --> 00:15:03,840
So yeah. So right now we have the meeting right after with Nora just to see what she

197
00:15:03,840 --> 00:15:12,160
should really need in order for us to make progress onto it. This ticket, we had some

198
00:15:12,160 --> 00:15:17,760
documents to delete in the S3 and trying to re-inject it. So I'll try and see if the re-injection

199
00:15:17,760 --> 00:15:27,600
this morning has made us have the, let's say, those documents back into the industry

200
00:15:27,600 --> 00:15:35,439
so that it can be flowed onto you like on all the tables and all. So it's something that I

201
00:15:35,439 --> 00:15:43,680
I will need to check on my side. So yeah. Mainly that will be it for me. I have something else to

202
00:15:43,680 --> 00:15:51,599
do on Squid, which is trying to build a transcript for a video, a demo video for

203
00:15:52,719 --> 00:15:59,599
for Squid because some users have requested a demo video of the application and I'm trying

204
00:15:59,599 --> 00:16:06,319
to check with the TNMD communication team how we can set up this. So I'll try and work on this today

205
00:16:06,480 --> 00:16:12,960
as well as the DistroCo deck that I will shut down with Christine.

206
00:16:14,320 --> 00:16:25,280
So that will be it. Tomorrow morning I'll be on training on data IQ. So I will not be available

207
00:16:25,280 --> 00:16:34,160
in the morning so I'll cancel the meeting. So maybe we'll catch it up like I think on Wednesday.

208
00:16:37,040 --> 00:16:42,720
So tomorrow morning you won't, so only Anoa is available? Yeah only Anoa will be available on

209
00:16:42,720 --> 00:16:49,840
our site so I'll check with Christine if she still wants to show the daily. Yeah I can keep it if you

210
00:16:49,840 --> 00:16:58,560
like. If not I'll cancel it. If you want to do it, it depends on you. Do you want me to put a message

211
00:16:58,560 --> 00:17:06,879
in the chat? Yeah we can just see if Christine still wants to hold it. Okay sure, let's do that.

212
00:17:08,399 --> 00:17:12,480
Okay thank you guys. Thank you. See you later. Bye.

