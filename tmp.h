UPDATE dri
SET dri.dri_input_date = SYSDATE
WHERE dri.status = '003' 
  AND dri.dri_code IN ('1001', '1002')
  AND EXISTS (
    SELECT 1 
    FROM dri_histo 
    WHERE dri_histo.dri_input_date = dri.dri_input_date
  );

UPDATE dri_histo
SET dri_histo.dri_input_date = SYSDATE
WHERE EXISTS (
    SELECT 1 
    FROM dri 
    WHERE dri.status = '003' 
      AND dri.dri_code IN ('1001', '1002')
      AND dri_histo.dri_input_date = dri.dri_input_date
  );
