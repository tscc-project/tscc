interface Props58 { value: number; label: string }
export const View58 = (p: Props58) => (
  <section data-i="58">
    <h2>{p.label}</h2>
    {p.value > 58 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
