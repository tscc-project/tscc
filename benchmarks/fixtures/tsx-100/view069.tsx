interface Props69 { value: number; label: string }
export const View69 = (p: Props69) => (
  <section data-i="69">
    <h2>{p.label}</h2>
    {p.value > 69 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
