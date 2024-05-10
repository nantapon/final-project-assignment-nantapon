
import { writable } from "svelte/store";

const STATUS_URI = "http://localhost:8080/status";
const PROFILES_URI = "http://localhost:8080/profiles";

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
        value: "Mbps",
        label: "Mbit/s"
    },
    bandwidth_unit: "Mbps",
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
    } else {
        delete x[n];
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
    for (const n in numberFieldNames) {
        setNumber(profile, n);
    }

    profile.ecn = profile.ecn ? true : false;
    profile.delay_unit = profile.delay_unit_ui.value;
    profile.bandwidth_unit = profile.bandwidth_unit_ui.value;
    console.log(status);

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