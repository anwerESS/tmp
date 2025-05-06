public class LocalDateTimeUtcSerializer extends StdSerializer<LocalDateTime> {
    public LocalDateTimeUtcSerializer() {
        super(LocalDateTime.class);
    }

    @Override
    public void serialize(LocalDateTime value, JsonGenerator gen, SerializerProvider provider) throws IOException {
        gen.writeString(value.atOffset(ZoneOffset.UTC).format(DateTimeFormatter.ISO_OFFSET_DATE_TIME));
    }
}

====================================================
    public class MyDto {
    @JsonSerialize(using = LocalDateTimeUtcSerializer.class)
    private LocalDateTime timestamp;
}
