interface Props21 { value: number; label: string }
export const View21 = (p: Props21) => (
  <section data-i="21">
    <h2>{p.label}</h2>
    {p.value > 21 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
