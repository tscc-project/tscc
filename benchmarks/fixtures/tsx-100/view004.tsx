interface Props4 { value: number; label: string }
export const View4 = (p: Props4) => (
  <section data-i="4">
    <h2>{p.label}</h2>
    {p.value > 4 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
