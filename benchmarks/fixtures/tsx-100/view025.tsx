interface Props25 { value: number; label: string }
export const View25 = (p: Props25) => (
  <section data-i="25">
    <h2>{p.label}</h2>
    {p.value > 25 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
