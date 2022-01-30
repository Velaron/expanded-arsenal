package su.xash.expandedarsenal;

import android.content.ComponentName;
import android.content.Intent;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Bundle;
import android.widget.ArrayAdapter;
import android.widget.AutoCompleteTextView;

import androidx.appcompat.app.AppCompatActivity;
import androidx.coordinatorlayout.widget.CoordinatorLayout;

import com.android.volley.Request;
import com.android.volley.RequestQueue;
import com.android.volley.toolbox.JsonObjectRequest;
import com.android.volley.toolbox.Volley;
import com.google.android.material.dialog.MaterialAlertDialogBuilder;
import com.google.android.material.floatingactionbutton.ExtendedFloatingActionButton;
import com.google.android.material.snackbar.Snackbar;
import com.google.android.material.textfield.TextInputEditText;
import com.google.android.material.textfield.TextInputLayout;

import org.json.JSONException;

public class LauncherActivity extends AppCompatActivity {
	private static final int XASH_NEW_MIN_VERSION = 1710;
	private static final int XASH_OLD_MIN_VERSION = 1200;
	private static final int NEW_ENGINE = 0;
	private static final int OLD_ENGINE = 1;
	private static final int OLD_ENGINE_TEST = 2;

	private static final String[] pkgs = {"su.xash.engine", "in.celest.xash3d.hl", "in.celest.xash3d.hl.test"};
	private static final String[] components = {"su.xash.engine.XashActivity", "in.celest.xash3d.XashActivity", "in.celest.xash3d.XashActivity"};

	@Override
	public void onCreate(Bundle savedInstanceBundle) {
		super.onCreate(savedInstanceBundle);
		setContentView(R.layout.activity_launcher);

		ExtractAssets.extractPAK(this, false);

		TextInputEditText launchParameters = findViewById(R.id.launchParameters);
		launchParameters.setText("-log -dev 2");

		ExtendedFloatingActionButton launchButton = findViewById(R.id.launchButton);
		launchButton.setOnClickListener((view) -> {
			String pkg = pkgs[NEW_ENGINE], cls = components[NEW_ENGINE];
			AutoCompleteTextView engineType = findViewById(R.id.engineType);

			if (engineType.getText().toString().equals(getResources().getStringArray(R.array.engine_types)[NEW_ENGINE])) {
				pkg = pkgs[NEW_ENGINE];
				cls = components[NEW_ENGINE];
			} else if (engineType.getText().toString().equals(getResources().getStringArray(R.array.engine_types)[OLD_ENGINE])) {
				pkg = pkgs[OLD_ENGINE];
				cls = components[OLD_ENGINE];
			} else if (engineType.getText().toString().equals(getResources().getStringArray(R.array.engine_types)[OLD_ENGINE_TEST])) {
				pkg = pkgs[OLD_ENGINE_TEST];
				cls = components[OLD_ENGINE_TEST];
			}

			startActivity(new Intent().setComponent(new ComponentName(pkg, cls))
					.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
					.putExtra("pakfile", getExternalFilesDir(null).getAbsolutePath() + "/extras.pak")
					.putExtra("gamedir", "hl_expanded_arsenal")
					.putExtra("argv", launchParameters.getText())
					.putExtra("gamelibdir", getApplicationInfo().nativeLibraryDir));
		});

		launchButton.setEnabled(false);

		checkForEngines();
	}

	private boolean checkForEngine(String packageName, int minVersion, String updateUrl) {
		try {
			PackageInfo info = getPackageManager().getPackageInfo(packageName, 0);

			if (info.versionCode < minVersion) {
				openDialog(updateUrl, getString(R.string.update_required), getString(R.string.update_available, "Xash3D FWGS"));
			} else {
				return true;
			}
		} catch (PackageManager.NameNotFoundException e) {
			// openDialog(updateUrl, getString(R.string.engine_not_found), getString(R.string.engine_info));
			return false;
		}

		return false;
	}

	private void checkForEngines() {
		ArrayAdapter<String> arrayAdapter = new ArrayAdapter<>(this, R.layout.list_item);

		if (checkForEngine(pkgs[NEW_ENGINE], XASH_NEW_MIN_VERSION, "https://github.com/FWGS/xash3d-fwgs/releases/tag/continuous")) {
			arrayAdapter.add(getResources().getStringArray(R.array.engine_types)[NEW_ENGINE]);
		}
		if (checkForEngine(pkgs[OLD_ENGINE], XASH_OLD_MIN_VERSION, "https://github.com/FWGS/xash3d/releases/tag/v0.19.2")) {
			arrayAdapter.add(getResources().getStringArray(R.array.engine_types)[OLD_ENGINE]);
		}
		if (checkForEngine(pkgs[OLD_ENGINE_TEST], 0, "https://github.com/FWGS/xash3d")) {
			arrayAdapter.add(getResources().getStringArray(R.array.engine_types)[OLD_ENGINE_TEST]);
		}

		if (arrayAdapter.isEmpty()) {
			new MaterialAlertDialogBuilder(LauncherActivity.this)
					.setTitle(R.string.engine_not_installed)
					.setMessage(R.string.should_install_engine)
					.setCancelable(false)
					.setNegativeButton(R.string.exit, (dialog, which) -> finish()).show();
		} else {
			AutoCompleteTextView engineType = findViewById(R.id.engineType);
			engineType.setAdapter(arrayAdapter);
			engineType.setText(arrayAdapter.getItem(0), false);

			if (arrayAdapter.getCount() < 2) {
				TextInputLayout engineTypeLayout = findViewById(R.id.engineTypeLayout);
				engineTypeLayout.setEnabled(false);
			}

			checkForUpdates();
		}
	}

	private void checkForUpdates() {
		String url = "https://api.github.com/repos/Velaron/expanded-arsenal/commits/master";
		CoordinatorLayout contextView = findViewById(R.id.coordinatorLayout);
		ExtendedFloatingActionButton launchButton = findViewById(R.id.launchButton);

		Snackbar updateNotification = Snackbar.make(contextView, R.string.checking_for_updates, Snackbar.LENGTH_INDEFINITE).setAnchorView(launchButton);
		updateNotification.show();

		JsonObjectRequest jsonObjectRequest = new JsonObjectRequest(Request.Method.GET, url, null, response -> {
			try {
				String sha = response.getString("sha").substring(0, 7);
				String version_sha = getPackageManager().getPackageInfo(getPackageName(), 0).versionName.split("-")[1];

				if (version_sha.equals(sha)) {
					updateNotification.dismiss();
					launchButton.setEnabled(true);
				} else {
					updateNotification.dismiss();
					openDialog("https://github.com/Velaron/expanded-arsenal/releases/tag/continuous", getString(R.string.update_required),
							getString(R.string.update_available, "Expanded Arsenal"));
				}
			} catch (JSONException e) {
				e.printStackTrace();
			} catch (PackageManager.NameNotFoundException e) {
				e.printStackTrace();
			}
		}, error -> {
			updateNotification.dismiss();
			launchButton.setEnabled(true);
		});

		RequestQueue requestQueue = Volley.newRequestQueue(getApplicationContext());
		requestQueue.add(jsonObjectRequest);
	}

	private void openDialog(String url, String title, String description) {
		new MaterialAlertDialogBuilder(LauncherActivity.this)
				.setTitle(title)
				.setMessage(description)
				.setCancelable(false)
				.setNegativeButton(R.string.exit, (dialog, which) -> finish())
				.setPositiveButton(R.string.update, (dialog, which) -> startActivity(new Intent(Intent.ACTION_VIEW).setData(Uri.parse(url)))).show();
	}
}