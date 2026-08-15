interface Props46 { value: number; label: string }
export const View46 = (p: Props46) => (
  <section data-i="46">
    <h2>{p.label}</h2>
    {p.value > 46 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
