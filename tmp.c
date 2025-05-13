public class ParisTimeDeserializer extends JsonDeserializer<LocalDateTime> {
    private static final ZoneId PARIS_ZONE = ZoneId.of("Europe/Paris");
    private static final DateTimeFormatter FORMATTER = 
        DateTimeFormatter.ofPattern("yyyy-MM-dd'T'HH:mm:ss");

    @Override
    public LocalDateTime deserialize(JsonParser p, DeserializationContext ctxt) 
            throws IOException {
        String dateStr = p.getText();
        // Parse as Paris time, then convert to LocalDateTime
        return LocalDateTime.parse(dateStr, FORMATTER)
               .atZone(PARIS_ZONE)
               .toLocalDateTime();
    }
}
