interface Props70 { value: number; label: string }
export const View70 = (p: Props70) => (
  <section data-i="70">
    <h2>{p.label}</h2>
    {p.value > 70 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
