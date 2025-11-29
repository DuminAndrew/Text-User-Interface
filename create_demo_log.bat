@echo off
REM Create a demo log file for testing Log Analyzer

echo Creating demo.log with sample log entries...

(
echo [2024-01-15 08:00:00] INFO: Application started successfully
echo [2024-01-15 08:00:01] INFO: Loading configuration from config.json
echo [2024-01-15 08:00:02] DEBUG: Configuration loaded: {"host": "localhost", "port": 8080}
echo [2024-01-15 08:00:03] INFO: Database connection established
echo [2024-01-15 08:00:04] DEBUG: Executing query: SELECT * FROM users WHERE active = true
echo [2024-01-15 08:00:05] INFO: Server listening on port 8080
echo [2024-01-15 08:00:10] INFO: New connection from 192.168.1.100
echo [2024-01-15 08:00:11] DEBUG: Processing HTTP GET /api/users
echo [2024-01-15 08:00:12] INFO: Request processed successfully (200 OK^)
echo [2024-01-15 08:00:15] WARNING: Connection pool usage at 75%%
echo [2024-01-15 08:00:20] ERROR: Failed to connect to cache server: Connection timeout
echo [2024-01-15 08:00:21] INFO: Retrying connection to cache server
echo [2024-01-15 08:00:22] INFO: Cache server connection restored
echo [2024-01-15 08:00:30] INFO: New connection from 192.168.1.101
echo [2024-01-15 08:00:31] DEBUG: Processing HTTP POST /api/login
echo [2024-01-15 08:00:32] INFO: User authentication successful: user@example.com
echo [2024-01-15 08:00:33] DEBUG: Generating session token
echo [2024-01-15 08:00:34] INFO: Session created: session_id=abc123xyz
echo [2024-01-15 08:00:40] WARNING: High CPU usage detected: 85%%
echo [2024-01-15 08:00:45] ERROR: Database query timeout after 30s
echo [2024-01-15 08:00:46] ERROR: Query: SELECT * FROM orders WHERE date > '2024-01-01' AND status IN ('pending', 'processing'^)
echo [2024-01-15 08:00:47] WARNING: Slow query detected (30.5s^)
echo [2024-01-15 08:00:50] INFO: Background job started: data_cleanup
echo [2024-01-15 08:00:55] DEBUG: Cleaned up 150 expired sessions
echo [2024-01-15 08:01:00] INFO: Memory usage: 512MB / 2048MB (25%%^)
echo [2024-01-15 08:01:05] ERROR: Invalid JSON received: {"name": "test", "value": }
echo [2024-01-15 08:01:06] WARNING: Request rejected due to invalid format
echo [2024-01-15 08:01:10] INFO: New connection from 192.168.1.102
echo [2024-01-15 08:01:11] DEBUG: Processing HTTP GET /api/products
echo [2024-01-15 08:01:12] INFO: Returned 50 products (200 OK^)
echo [2024-01-15 08:01:20] CRITICAL: Out of memory exception
echo [2024-01-15 08:01:21] ERROR: Failed to allocate 100MB buffer
echo [2024-01-15 08:01:22] INFO: Triggering garbage collection
echo [2024-01-15 08:01:23] INFO: Memory recovered: 300MB freed
echo [2024-01-15 08:01:30] INFO: Health check passed
echo [2024-01-15 08:01:35] DEBUG: Active connections: 15
echo [2024-01-15 08:01:40] WARNING: Rate limit exceeded for IP 192.168.1.105
echo [2024-01-15 08:01:41] INFO: Request blocked (429 Too Many Requests^)
echo [2024-01-15 08:01:50] INFO: New connection from 192.168.1.103
echo [2024-01-15 08:01:51] DEBUG: Processing HTTP PUT /api/users/123
echo [2024-01-15 08:01:52] INFO: User profile updated successfully
echo [2024-01-15 08:02:00] ERROR: External API call failed: https://api.example.com/data
echo [2024-01-15 08:02:01] WARNING: Falling back to cached data
echo [2024-01-15 08:02:05] INFO: Cache hit rate: 92%%
echo [2024-01-15 08:02:10] DEBUG: Running scheduled task: backup_logs
echo [2024-01-15 08:02:15] INFO: Log backup completed: 50MB compressed to 5MB
echo [2024-01-15 08:02:20] INFO: System status: All services operational
) > demo.log

echo Demo log file created successfully: demo.log
echo Total lines: 47
