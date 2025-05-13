public class ParisTimeToUtcDeserializer extends JsonDeserializer<LocalDateTime> {
    private static final ZoneId PARIS_ZONE = ZoneId.of("Europe/Paris");
    private static final DateTimeFormatter FORMATTER = 
        DateTimeFormatter.ofPattern("yyyy-MM-dd'T'HH:mm:ss");

    @Override
    public LocalDateTime deserialize(JsonParser p, DeserializationContext ctxt) 
            throws IOException {
        // 1. Parse input as Paris time
        ZonedDateTime parisTime = LocalDateTime.parse(p.getText(), FORMATTER)
                                .atZone(PARIS_ZONE);
        
        // 2. Convert to UTC and return as LocalDateTime (drops timezone)
        return parisTime.withZoneSameInstant(ZoneOffset.UTC).toLocalDateTime();
    }
}
