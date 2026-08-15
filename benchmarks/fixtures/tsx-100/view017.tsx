interface Props17 { value: number; label: string }
export const View17 = (p: Props17) => (
  <section data-i="17">
    <h2>{p.label}</h2>
    {p.value > 17 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
