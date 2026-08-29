#!/usr/bin/env python3
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
matrix = json.loads((ROOT / "docs/feature-matrix.json").read_text())
cases = json.loads((ROOT / "regression/cases.json").read_text())

allowed = {"supported", "partial", "planned", "not-applicable"}
dimensions = ["parse", "bind", "type", "lower", "emit", "runtime", "diagnostic", "project"]
assert matrix["schema_version"] == 1
assert matrix["dimensions"] == dimensions
assert matrix["oracle"]["cases"] == len(cases)
assert matrix["oracle"]["pass"] + matrix["oracle"]["skip"] == len(cases)
assert matrix["oracle"]["fail"] == 0

case_names = {case["name"] for case in cases}
feature_ids = set()
for feature in matrix["features"]:
    assert feature["id"] not in feature_ids
    feature_ids.add(feature["id"])
    assert set(feature["status"]) == set(dimensions)
    assert set(feature["status"].values()) <= allowed
    assert feature["cases"], feature["id"]
    missing = set(feature["cases"]) - case_names
    assert not missing, f"{feature['id']}: unknown cases {sorted(missing)}"

print(f"tscc feature matrix valid: {len(feature_ids)} families, {len(cases)} regression cases")
