SELECT d.dri_code, d.dri_input_date
FROM dri d
JOIN dri_histo h ON d.dri_code = h.dri_code AND d.dri_input_date = h.dri_input_date
WHERE d.sta_id = '003' AND d.dri_code IN ('1001', '1002')
