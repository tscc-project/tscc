interface Props22 { value: number; label: string }
export const View22 = (p: Props22) => (
  <section data-i="22">
    <h2>{p.label}</h2>
    {p.value > 22 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
