interface Props2 { value: number; label: string }
export const View2 = (p: Props2) => (
  <section data-i="2">
    <h2>{p.label}</h2>
    {p.value > 2 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
