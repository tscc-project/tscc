interface Props92 { value: number; label: string }
export const View92 = (p: Props92) => (
  <section data-i="92">
    <h2>{p.label}</h2>
    {p.value > 92 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
