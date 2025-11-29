#!/bin/bash
# Create a demo log file for testing Log Analyzer

echo "Creating demo.log with sample log entries..."

cat > demo.log << 'EOF'
[2024-01-15 08:00:00] INFO: Application started successfully
[2024-01-15 08:00:01] INFO: Loading configuration from config.json
[2024-01-15 08:00:02] DEBUG: Configuration loaded: {"host": "localhost", "port": 8080}
[2024-01-15 08:00:03] INFO: Database connection established
[2024-01-15 08:00:04] DEBUG: Executing query: SELECT * FROM users WHERE active = true
[2024-01-15 08:00:05] INFO: Server listening on port 8080
[2024-01-15 08:00:10] INFO: New connection from 192.168.1.100
[2024-01-15 08:00:11] DEBUG: Processing HTTP GET /api/users
[2024-01-15 08:00:12] INFO: Request processed successfully (200 OK)
[2024-01-15 08:00:15] WARNING: Connection pool usage at 75%
[2024-01-15 08:00:20] ERROR: Failed to connect to cache server: Connection timeout
[2024-01-15 08:00:21] INFO: Retrying connection to cache server
[2024-01-15 08:00:22] INFO: Cache server connection restored
[2024-01-15 08:00:30] INFO: New connection from 192.168.1.101
[2024-01-15 08:00:31] DEBUG: Processing HTTP POST /api/login
[2024-01-15 08:00:32] INFO: User authentication successful: user@example.com
[2024-01-15 08:00:33] DEBUG: Generating session token
[2024-01-15 08:00:34] INFO: Session created: session_id=abc123xyz
[2024-01-15 08:00:40] WARNING: High CPU usage detected: 85%
[2024-01-15 08:00:45] ERROR: Database query timeout after 30s
[2024-01-15 08:00:46] ERROR: Query: SELECT * FROM orders WHERE date > '2024-01-01' AND status IN ('pending', 'processing')
[2024-01-15 08:00:47] WARNING: Slow query detected (30.5s)
[2024-01-15 08:00:50] INFO: Background job started: data_cleanup
[2024-01-15 08:00:55] DEBUG: Cleaned up 150 expired sessions
[2024-01-15 08:01:00] INFO: Memory usage: 512MB / 2048MB (25%)
[2024-01-15 08:01:05] ERROR: Invalid JSON received: {"name": "test", "value": }
[2024-01-15 08:01:06] WARNING: Request rejected due to invalid format
[2024-01-15 08:01:10] INFO: New connection from 192.168.1.102
[2024-01-15 08:01:11] DEBUG: Processing HTTP GET /api/products
[2024-01-15 08:01:12] INFO: Returned 50 products (200 OK)
[2024-01-15 08:01:20] CRITICAL: Out of memory exception
[2024-01-15 08:01:21] ERROR: Failed to allocate 100MB buffer
[2024-01-15 08:01:22] INFO: Triggering garbage collection
[2024-01-15 08:01:23] INFO: Memory recovered: 300MB freed
[2024-01-15 08:01:30] INFO: Health check passed
[2024-01-15 08:01:35] DEBUG: Active connections: 15
[2024-01-15 08:01:40] WARNING: Rate limit exceeded for IP 192.168.1.105
[2024-01-15 08:01:41] INFO: Request blocked (429 Too Many Requests)
[2024-01-15 08:01:50] INFO: New connection from 192.168.1.103
[2024-01-15 08:01:51] DEBUG: Processing HTTP PUT /api/users/123
[2024-01-15 08:01:52] INFO: User profile updated successfully
[2024-01-15 08:02:00] ERROR: External API call failed: https://api.example.com/data
[2024-01-15 08:02:01] WARNING: Falling back to cached data
[2024-01-15 08:02:05] INFO: Cache hit rate: 92%
[2024-01-15 08:02:10] DEBUG: Running scheduled task: backup_logs
[2024-01-15 08:02:15] INFO: Log backup completed: 50MB compressed to 5MB
[2024-01-15 08:02:20] INFO: System status: All services operational
EOF

echo "Demo log file created successfully: demo.log"
echo "Total lines: $(wc -l < demo.log)"
