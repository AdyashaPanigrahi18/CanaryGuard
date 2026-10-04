# Test Plan

1. Start `canary_monitor.exe`.
2. Press `S` and verify the three files show `NORMAL [OK]`.
3. Press `T` for the safe simulation.
4. Verify modification/deletion alerts appear.
5. Verify the files are restored automatically.
6. Press `S` again.
7. Check `logs/alerts.log`.
