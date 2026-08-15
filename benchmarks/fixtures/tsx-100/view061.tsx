interface Props61 { value: number; label: string }
export const View61 = (p: Props61) => (
  <section data-i="61">
    <h2>{p.label}</h2>
    {p.value > 61 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
