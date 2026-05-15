import os
import pytest

from edge_kg import EdgeKnowledgeGraph


def test_synonym_mapping(tmp_path):
    cfg_dir = tmp_path / "kg_config"
    cfg_dir.mkdir()

    # create minimal ontology and synonym files
    (cfg_dir / "ontology.json").write_text(
        '{"ontology_graph": {"drink": ["cup"], "eat": ["apple"]}, "stop_words": ["to"]}'
    )
    (cfg_dir / "synonym_map.json").write_text('{"sip": "drink", "consume": "eat"}')

    kg = EdgeKnowledgeGraph(config_dir=str(cfg_dir), max_prompt_chars=100)

    # use synonym 'sip' -> mapped to 'drink'
    assert kg.resolve_prompt("sip some water") == ["cup"]


def test_truncation():
    long_input = "a" * 500
    kg = EdgeKnowledgeGraph(config_dir=None, max_prompt_chars=10)
    # No tokens, but ensure it doesn't crash and returns empty list
    assert kg.resolve_prompt(long_input) == []


def test_default_ontology():
    kg = EdgeKnowledgeGraph()
    res = kg.resolve_prompt("eat apple")
    assert "apple" in res
