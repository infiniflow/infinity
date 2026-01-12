# Infinity GUI

Web-based management interface for Infinity database.

## Prerequisites

- **Node.js** 18+
- **pnpm** package manager
- **Infinity server** running on `http://127.0.0.1:23820`

## Quick Start

### 1. Install Dependencies

```bash
pnpm install
```

### 2. Configure Environment

Copy the example environment file and generate a secret:

```bash
# Copy example configuration
cp .env.example .env.local

# Generate a secret key
openssl rand -base64 32

# Edit .env.local and replace AUTH_SECRET with the generated value
```

**Note:** The `AUTH_SECRET` is required for NextAuth to work properly. Each installation should use a unique secret.

### 3. Start Development Server

```bash
pnpm dev
```

The GUI will be available at: **http://localhost:3033**

### 4. Start Infinity Server

Make sure the Infinity server is running:

```bash
# From the infinity root directory
./cmake-build-debug/src/infinity
```

## Configuration

### Environment Variables

| Variable | Description | Required |
|----------|-------------|----------|
| `AUTH_SECRET` | Secret key for NextAuth (generate with `openssl rand -base64 32`) | Yes |
| `GITHUB_CLIENT_ID` | GitHub OAuth client ID (optional) | No |
| `GITHUB_CLIENT_SECRET` | GitHub OAuth client secret (optional) | No |

### API Configuration

The GUI connects to Infinity at: `http://127.0.0.1:23820`

To change this, edit `lib/request.ts`:
```typescript
const baseUrl = 'http://your-infinity-server:port/';
```

## Development

```bash
# Development mode (port 3033)
pnpm dev

# Build for production
pnpm build

# Start production server (port 3043)
pnpm start
```

## Features

- Database management (create, list, drop)
- Table structure viewing (columns, indexes, segments)
- System configuration monitoring
- Real-time database tree view

## Troubleshooting

### "MissingSecret" Error

If you see `MissingSecret: Please define a secret`, make sure:
1. `.env.local` file exists
2. `AUTH_SECRET` is set with a valid value

### Cannot Connect to Infinity

1. Check if Infinity server is running: `curl http://127.0.0.1:23820/databases`
2. Verify the port in `lib/request.ts`
3. Check firewall settings

### Table List Empty

1. Click the arrow next to database name to expand
2. Tables load lazily when you expand the database node
