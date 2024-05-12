
import { writable } from "svelte/store";

let STATUS_URI;
let PROFILES_URI;
if (location.port === "5173") {
    STATUS_URI = "http://localhost:8080/status";
    PROFILES_URI = "http://localhost:8080/profiles";
} else {
    STATUS_URI = "/status";
    PROFILES_URI = "/profiles";
}

export const status = writable({
    enabled: false,
    profile: {}
});
export const profiles = writable({});
export const defaultProfile = {
    ecn: false,
    loss_model: "random",
    delay_unit_ui: {
        value: "ms",
        label: "ms"
    },
    delay_unit: "ms",
    jitter_unit_ui: {
        value: "ms",
        label: "ms"
    },
    jitter_unit: "ms",
    delay_dist: "normal",
    bandwidth_unit_ui: {
        value: "mbit",
        label: "Mbit/s"
    },
    bandwidth_unit: "mbit",
    description: "",
    loss_enabled: false,
    delay_enabled: false,
    bandwidth_enabled: false,
    corrupt_enabled: false,
    duplicate_enabled: false,
    reorder_enabled: false
};

function addDefaultToStatus(status) {
    status.profile = {
        ...defaultProfile,
        ...status.profile
    };
}

export async function getStatus() {
    const res = await fetch(STATUS_URI);

    if (!res.ok) {
        throw new Error('Request failed');
    }

    let status = await res.json();
    addDefaultToStatus(status);
    return status;
}

function setNumber(x, n) {
    if (x[n]) {
        x[n] = Number(x[n]);
    }
}

const numberFieldNames = [
    "delay", "jitter", "delay_corr", "loss",
    "p13", "p31", "p32", "p23", "p14",
    "p", "r", "h_bar", "k_bar",
    "corrupt", "corrupt_corr",
    "duplicate", "duplicate_corr",
    "reorder", "reorder_corr", "reorder_gap",
    "bandwidth"
];

export async function saveStatus(status) {
    let profile = status.profile;
    for (const n of numberFieldNames) {
        setNumber(profile, n);
    }

    profile.ecn = profile.ecn ? true : false;
    profile.delay_unit = profile.delay_unit_ui.value;
    profile.bandwidth_unit = profile.bandwidth_unit_ui.value;
    status.profile = { ...profile };
    console.log("Status:", status);

    const res = await fetch(STATUS_URI, {
        method: "POST",
        body: JSON.stringify(status)
    });

    if (!res.ok) {
        throw new Error('Request failed');
    }

    let status1 = await res.json();
    addDefaultToStatus(status1);
    return status1;
}

export async function getProfiles() {
    const res = await fetch(PROFILES_URI);

    if (!res.ok) {
        throw new Error('Request failed');
    }

    let profiles = await res.json();
    return profiles;
}

export async function saveProfiles(profiles) {
    console.log("Profiles: ", profiles);

    const res = await fetch(PROFILES_URI, {
        method: "POST",
        body: JSON.stringify(profiles)
    });

    if (!res.ok) {
        throw new Error('Request failed');
    }

    let profiles1 = await res.json();
    return profiles1;
}