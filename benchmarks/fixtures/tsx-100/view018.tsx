interface Props18 { value: number; label: string }
export const View18 = (p: Props18) => (
  <section data-i="18">
    <h2>{p.label}</h2>
    {p.value > 18 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
