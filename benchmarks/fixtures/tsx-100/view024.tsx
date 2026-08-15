interface Props24 { value: number; label: string }
export const View24 = (p: Props24) => (
  <section data-i="24">
    <h2>{p.label}</h2>
    {p.value > 24 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
